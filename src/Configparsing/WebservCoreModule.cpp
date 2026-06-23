#include "../../inc/Configparsing/WebservCoreModule.hpp"

WebservCoreModule::WebservCoreModule(int& ctxIndex) :
	WebservCoreParser(ctxIndex) {
};

void    WebservCoreModule::initConfIfEmptyAtLevel(const ConfCtx& confCtx,
    WebservConfLevel level, const LocNode* locNodePtr) {
	_initConfIfEmptyAtLevel<HttpCoreConf, SrvCoreConf, LocCoreConf>(
		confCtx, level, locNodePtr);
}
