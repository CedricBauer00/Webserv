#include "../../inc/Configparsing/WebservRedirectModule.hpp"

WebservRedirectModule::WebservRedirectModule(int& ctxIndex) :
	WebservRedirectParser(ctxIndex) {
}

void	WebservRedirectModule::initConfIfEmptyAtLevel(const ConfCtx& confCtx,
    WebservConfLevel level, const LocNode* locNodePtr) {
    _initConfIfEmptyAtLevel<HttpRedirectConf, SrvRedirectConf, LocRedirectConf>(
        confCtx, level, locNodePtr);
}
