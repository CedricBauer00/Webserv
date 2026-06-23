#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"
#include "Method.hpp"

class	Executor: public AEventHandler {
	private:
		HttpParser			_parser;
		std::function<const Srv*(const std::string&)> _selectServer;
		Response			_res;
		const LocNode*		_loc{nullptr};
		const LocCoreConf*	_locCoreConf{nullptr};
		const LocIndexConf*	_locIndexConf{nullptr};
		std::string			_fsPath;

		void	_resolveLocConfs();
		void	_selectLocation(const LocNode& root);
		void	_setWorkingDirAsFilesystemPath();
		void	_assertHttpMethodAllowed();
		void	_resolveFilesystemPath();

	public:
		Executor() = delete;
		Executor(AEventHandler& handler,
			HttpParser&& parser,
            std::function<const Srv*(const std::string&)>&& selectServer);
		virtual ~Executor();

		void	process(uint32_t events) override;
};
