/****************************************************************************
          Copyright (C) 1999, Karl Franzens University, Graz

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND THE KARL FRANZENS
          UNIVERSITY DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          owned by the Karl Franzens University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include this copyright notice.
****************************************************************************/

/***************************************************************************
 *
 * Karl Franzens University, Graz
 * UCDMC2 DICOM Network Transport Libraries 
 * Version 0.1 Beta
 *
 * Technical Contact: gunter.zeilinger@kfunigraz.ac.at
 *
 ***************************************************************************/

#include "Dicom.h"
#include <iostream>

using std::cout;
using std::cin;


class StorageHandler : public CHandler  
{
public:
	virtual UINT16 handle(const DataSet& rq, const DataSet& data, Sequence* sq);
};

UINT16 StorageHandler::handle(const DataSet &rq, const DataSet &data, Sequence *sq)
{
	cout << "Received image data:\n";
	Dumper dumper(cout);
	dumper.write(data);
	return 0;
}

class Wklisthandler : public CHandler  
{
public:
	virtual UINT16 handle(const DataSet& rq, const DataSet& data, Sequence* sq);
};

UINT16 Wklisthandler::handle(const DataSet &rq, const DataSet &data, Sequence *sq)
{
	cout << "Received key data:\n";
	Dumper dumper(cout);
	dumper.write(data);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "usage: server <port>\n";
		return -1;
	}

	DefMsgLogger log(cout);
	MsgLogger::set(&log);
	DefErrLogger err(cout);
	ErrLogger::set(&err);
	Server server;
	StorageHandler storehandler;
	Wklisthandler wklisthandler;
	server.addCStoreHandler(UID_CR_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCStoreHandler(UID_CT_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCStoreHandler(UID_MR_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCStoreHandler(UID_NM_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCStoreHandler(UID_US_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCStoreHandler(UID_SC_IMAGE_STORAGE_SOP_CLASS, &storehandler);
	server.addCFindHandler(UID_MODALITY_WORKLIST_SOP_CLASS, &wklisthandler);
	server.runMultiThread(argv[1]);

	return 0;
}
