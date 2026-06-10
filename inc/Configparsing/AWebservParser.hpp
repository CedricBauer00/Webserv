#pragma once

#include <unordered_map>
#include <stdexcept>
#include <functional>
#include "IWebservModule.hpp"

class AWebservParser : virtual public IWebservModule {
	public:
		using parseFunc = std::function<void(const std::string& d, Tokens& t,
			const ConfCtx& c, WebservConfLevel l, ConfigParser& p)>;

    private:
        template<typename T>
		void	_insertConf(VecOfPtrs<T>* confs, std::unique_ptr<T> conf);

    protected:
        const int	_ctxIndex;
        // const std::unordered_map<std::string, WebservConfLevel>
		// 	_directiveValLevelMap;

        template<typename T, typename Factory>
        void	_ensureConfExists(VecOfPtrs<T>* confs, Factory makeConf);

		template<typename HttpConfType,
				typename SrvConfType,
				typename LocConfType>
		void	_initConfIfEmptyAtLevel(
			const ConfCtx& confCtx, WebservConfLevel level);

		bool	_isDelimiter(const std::string& tok);
		bool	_parseBooleanValue(const std::string& tok);
		void	_addLowerLevelDirective(const std::string& directive,
			std::vector<std::string> vals,
            std::vector<std::vector<std::string>>& arr);
		virtual const std::unordered_map<
		std::string, std::pair<WebservConfLevel, parseFunc>>&	_getParseMap() = 0;

    public:
        AWebservParser() = delete;
        AWebservParser(int& ctxIndex);
        virtual ~AWebservParser() = default;

        int			isDirectiveValid(const std::string& directive,
			WebservConfLevel level) override;
        HttpConf*	getHttpConfPtr(const ConfCtx& confCtx) override;
		SrvConf*	getSrvConfPtr(const ConfCtx& confCtx) override;
		LocConf*	getLocConfPtr(const ConfCtx& confCtx) override;

};

template<typename T>
void AWebservParser::_insertConf(VecOfPtrs<T>* confs, std::unique_ptr<T> conf)
{
    confs->resize(_ctxIndex + 1);
    (*confs)[_ctxIndex] = std::move(conf);
}

template<typename T, typename Factory>
void AWebservParser::_ensureConfExists(VecOfPtrs<T>* confs, Factory makeConf) {
	if (confs->size() <= static_cast<size_t>(_ctxIndex))
		_insertConf<T>(confs, makeConf());
};

template<typename HttpConfType, typename SrvConfType, typename LocConfType>
void AWebservParser::_initConfIfEmptyAtLevel(
	const ConfCtx& confCtx, WebservConfLevel level) {
	switch (level) {
		case WebservConfLevel::HTTP:
			_ensureConfExists(confCtx.httpConfs,
							[]()
							{ return std::make_unique<HttpConfType>(); });
			break;
		case WebservConfLevel::SERVER:
			_ensureConfExists(confCtx.srvConfs,
							[]()
							{ return std::make_unique<SrvConfType>(); });
			[[fallthrough]];
		case WebservConfLevel::LOCATION:
			_ensureConfExists(confCtx.locConfs,
							[]()
							{ return std::make_unique<LocConfType>(); });
			break;
		default:
			throw std::runtime_error("Invalid configuration level");
			break;
	};
};
