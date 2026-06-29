#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"
#include "Method.hpp"

class	Executor: public AEventHandler {
	private:
		HttpParser				_parser;
		Response				_res;
		const LocNode*			_loc{nullptr};
		const LocCoreConf*		_locCoreConf{nullptr};
		const LocIndexConf*		_locIndexConf{nullptr};
		const LocRedirectConf*	_locRedirectConf{nullptr};
		std::string				_fsPath;

		void	_resolveLocConfs();
		void	_selectLocation(const LocNode& root);
		void	_setWorkingDirAsFilesystemPath();
		void	_assertHttpMethodAllowed();
		void	_resolveFilesystemPath();

	public:
		Executor() = delete;
		Executor(AEventHandler&& handler,
			HttpParser&& parser,
			Response&& res);
		virtual ~Executor();

		void	process(uint32_t events) override;
};
