#include "../../inc/Configparsing/WebservIndexModule.hpp"

WebservIndexModule::WebservIndexModule(int& ctxIndex) :
	WebservIndexParser(ctxIndex) {
};

void    WebservIndexModule::initConfIfEmptyAtLevel(const ConfCtx& confCtx,
    WebservConfLevel level, const LocNode* locNodePtr) {
    _initConfIfEmptyAtLevel<HttpIndexConf, SrvIndexConf, LocIndexConf>(
        confCtx, level, locNodePtr);
}
