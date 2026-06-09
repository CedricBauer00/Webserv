#pragma once

#include "HeadReader.hpp"
#include "Response.hpp"
#include "Method.hpp"

class	Executor: public AEventHandler {
	private:
		HttpParser	_parser;
		std::function<const IWebservModule::Srv*(const std::string&)> _selectServer;
		Response	_res;

	public:
		Executor() = delete;
		Executor(const AEventHandler& handler,
			HttpParser&& parser,
            std::function<const IWebservModule::Srv*(const std::string&)>&& selectServer);
		virtual ~Executor();

		static const IWebservModule::LocNode*	selectLocation(const std::string& path,
            const IWebservModule::LocNode& root);
		void	process(uint32_t events) override;
};
