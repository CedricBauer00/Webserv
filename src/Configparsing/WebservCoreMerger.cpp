#include <filesystem>
#include "../../inc/Configparsing/WebservCoreMerger.hpp"
#include "../../inc/Configparsing/WebservCoreParser.hpp"

WebservCoreMerger::WebservCoreMerger() :
HttpCoreConf(),
SrvCoreConf({}, 10000, WebservMsec{1000}, true, true, true, 0),
LocCoreConf(11,
	std::filesystem::current_path().string(),
	false,
	{},
	1000,
	1000,
	WebservMsec{1000},
	WebservMsec{1000},
	true,
	false,
	false) {
}

WebservCoreMerger::~WebservCoreMerger() {
}

void	WebservCoreMerger::mergeConfs(ConfigParser& parser,
	std::unique_ptr<LocNode>& location) {
	inheritFromHttpConf<HttpCoreConf>(parser);
	assignDefaults(parser.getConfCtx(), *this, *this);
	print(parser.getConfCtx());
	(void)location;
}

void	WebservCoreMerger::print(const ConfCtx& ctx) {
	if (getHttpConfPtr(ctx) != nullptr)
		printHttpConf(*dynamic_cast<const HttpCoreConf*>(getHttpConfPtr(ctx)));
	if (getSrvConfPtr(ctx) != nullptr)
		printSrvConf(*dynamic_cast<const SrvCoreConf*>(getSrvConfPtr(ctx)));
	if (getLocConfPtr(ctx) != nullptr)
		printLocConf(*dynamic_cast<const LocCoreConf*>(getLocConfPtr(ctx)));
}

void	WebservCoreMerger::printHttpConf(const HttpCoreConf& httpConf) {
	std::cout << "----------HttpCoreConf----------" << std::endl;
	for (const auto& token : httpConf.lowerLevelDirectives)
		std::cout << token << " ";
	std::cout << std::endl;
}

void	WebservCoreMerger::printSrvConf(const SrvCoreConf& srvConf) {
	std::cout << "----------SrvCoreConf----------" << std::endl;
	std::cout << "serverNames: ";
	for (const auto& name : srvConf.serverNames)
		std::cout << name << " ";
	std::cout << " ; ";
	std::cout << "numReqExpected: "
		<< (srvConf.numReqExpected.has_value() ? std::to_string(srvConf.numReqExpected.value()) : "nullopt")
		<< " ; ";
	std::cout << "clientHeaderTimeout: "
		<< (srvConf.clientHeaderTimeout.has_value() ? std::to_string(srvConf.clientHeaderTimeout.value().count()) + "ms" : "nullopt")
		<< " ; ";
	std::cout << "ignore_invalid_headers: "
		<< (srvConf.ignore_invalid_headers.has_value() ? (srvConf.ignore_invalid_headers.value() ? "true" : "false") : "nullopt")
		<< " ; ";
	std::cout << "merge_slashes: "
		<< (srvConf.merge_slashes.has_value() ? (srvConf.merge_slashes.value() ? "true" : "false") : "nullopt")
		<< " ; ";
	std::cout << "underscore_is_valid: "
		<< (srvConf.underscore_is_valid.has_value() ? (srvConf.underscore_is_valid.value() ? "true" : "false") : "nullopt")
		<< " ; " << std::endl;
}	

void	WebservCoreMerger::printLocConf(const LocCoreConf& locConf) {
	std::cout << "----------LocCoreConf----------" << std::endl;
	std::cout << "nameLen: " << std::to_string(locConf.nameLen) << " ; ";
	std::cout << "allowedMethods: "
		<< (locConf.allowedMethods.has_value() ? std::to_string(locConf.allowedMethods.value()) : "nullopt")
		<< " ; ";
	std::cout << "root: " << locConf.root << " ; ";
	std::cout << "alias: "
		<< (locConf.alias.has_value() ? std::to_string(locConf.alias.value()) : "nullopt")
		<< " ; ";
	std::cout << "postRedirect: " << locConf.postRedirect << " ; ";
	std::cout << "clientMaxBodySize: "
		<< (locConf.clientMaxBodySize.has_value() ? std::to_string(locConf.clientMaxBodySize.value()) + " bytes" : "nullopt")
		<< " ; ";
	std::cout << "clientBodyBufferSize: "
		<< (locConf.clientBodyBufferSize.has_value() ? std::to_string(locConf.clientBodyBufferSize.value()) + " bytes" : "nullopt")
		<< " ; ";
	std::cout << "clientBodyTimeout: "
		<< (locConf.clientBodyTimeout.has_value() ? std::to_string(locConf.clientBodyTimeout.value().count()) + "ms" : "nullopt")
		<< " ; ";
	std::cout << "sendTimeout: "
		<< (locConf.sendTimeout.has_value() ? std::to_string(locConf.sendTimeout.value().count()) + "ms" : "nullopt")
		<< " ; ";
	std::cout << "absoluteRedirect: "
		<< (locConf.absoluteRedirect.has_value() ? (locConf.absoluteRedirect.value() ? "true" : "false") : "nullopt")
		<< " ; ";
	std::cout << "logNotFound: "
		<< (locConf.logNotFound.has_value() ? (locConf.logNotFound.value() ? "true" : "false") : "nullopt")
		<< " ; ";
	std::cout << "chunkedTransferEncoding: "
		<< (locConf.chunkedTransferEncoding.has_value() ? (locConf.chunkedTransferEncoding.value() ? "true" : "false") : "nullopt")
		<< " ; " << std::endl;
}
