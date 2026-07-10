#pragma once

#include <limits>
#include "HeadReader.hpp"
#include "Response.hpp"
#include "Method.hpp"

class	Executor: public AEventHandler {
	private:
		std::unique_ptr<AHttpParser>	_parser;
		std::unique_ptr<Response>		_res;
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
			std::unique_ptr<AHttpParser>&& parser,
			std::unique_ptr<Response>&& res);
		virtual ~Executor();

		void	process(uint32_t events) override;
};
