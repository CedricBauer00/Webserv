#pragma once

#include <unordered_map>
#include <stdexcept>
#include "IWebservModule.hpp"

class AWebservParser : virtual public IWebservModule {
    private:
        template<typename T>
		void	_insertConf(VecOfPtrs<T>* confs, std::unique_ptr<T> conf);
    protected:
        const int	_ctxIndex;
        const std::unordered_map<std::string, WebservConfLevel>
			_directiveValLevelMap;
        template<typename T, typename Factory>
        void	_ensureConfExists(VecOfPtrs<T>* confs, Factory makeConf);
    public:
        AWebservParser() = delete;
        AWebservParser(int& ctxIndex,
			std::unordered_map<std::string, WebservConfLevel> 
                directiveValLevelMap
		);
        virtual ~AWebservParser() = default;
        int	isDirectiveValid(const std::string& directive,
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
