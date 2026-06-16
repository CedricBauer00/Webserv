#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"
#include "Method.hpp"

class	Executor: public AEventHandler {
	private:
		HttpParser			_parser;
		std::function<const Srv*(const std::string&)> _selectServer;
		Response			_res;
		const LocNode*		_loc;
		const LocCoreConf*	_locCoreConf{nullptr};
		const LocIndexConf*	_locIndexConf{nullptr};
		std::string			_filesystemPath;

		void	_selectLocation(const std::string& path, const LocNode& root);
		void	_resolveLocConfs();
		void	_setWorkingDirAsPath();
		void	_assertHttpMethodAllowed();
		void	_resolveFilesystemPath();

	public:
		Executor() = delete;
		Executor(const AEventHandler& handler,
			HttpParser&& parser,
            std::function<const Srv*(const std::string&)>&& selectServer);
		virtual ~Executor();

		void	process(uint32_t events) override;
};
