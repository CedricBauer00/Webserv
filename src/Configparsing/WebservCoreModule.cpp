#include "../../inc/Configparsing/WebservCoreModule.hpp"

WebservCoreModule::WebservCoreModule(int& ctxIndex) :
	WebservCoreParser(ctxIndex) {
};

void    WebservCoreModule::initConfIfEmptyAtLevel(const ConfCtx& confCtx,
    WebservConfLevel level) {
	_initConfIfEmptyAtLevel<HttpCoreConf, SrvCoreConf, LocCoreConf>(
		confCtx, level);
}
