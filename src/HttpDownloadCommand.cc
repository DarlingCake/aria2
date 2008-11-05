/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "HttpDownloadCommand.h"
#include "RequestGroup.h"
#include "DownloadEngine.h"
#include "Request.h"
#include "HttpRequestCommand.h"
#include "HttpConnection.h"
#include "HttpRequest.h"
#include "Segment.h"
#include "Socket.h"
#include "prefs.h"
#include "Option.h"

namespace aria2 {

HttpDownloadCommand::HttpDownloadCommand(int cuid,
					 const RequestHandle& req,
					 RequestGroup* requestGroup,
					 const HttpConnectionHandle& httpConnection,
					 DownloadEngine* e,
					 const SocketHandle& socket)
  :DownloadCommand(cuid, req, requestGroup, e, socket),
   _httpConnection(httpConnection) {}

HttpDownloadCommand::~HttpDownloadCommand() {}

bool HttpDownloadCommand::prepareForNextSegment() {
  if(req->isPipeliningEnabled() && !_requestGroup->downloadFinished()) {
    Command* command = new HttpRequestCommand(cuid, req, _requestGroup, _httpConnection, e, socket);
    e->commands.push_back(command);
    return true;
  } else {
    if(req->isPipeliningEnabled() ||
       (req->isKeepAliveEnabled() &&
	((!_transferEncodingDecoder.isNull() &&
	  _requestGroup->downloadFinished()) ||
	 (uint64_t)_segments.front()->getPositionToWrite() ==
	 _requestGroup->getTotalLength()))) {
      e->poolSocket(req, isProxyDefined(), socket);
    }

    return DownloadCommand::prepareForNextSegment();
  }
}

} // namespace aria2
