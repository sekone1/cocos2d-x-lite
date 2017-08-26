// Sdk.h

#ifndef __SDK__CLASS

#define __SDK__CLASS

#include "cocos2d.h"
using namespace cocos2d;


namespace cocos2d {
    class Sdk : public cocos2d::Ref
    {
    public:
        enum class AdState
        {
            Completed,//已观看完
            Skipped,//跳过
            Error,
        };
        class Delegate : public cocos2d::Ref{
        public:
            /** Destructor of Delegate. */
            virtual ~Delegate() {}
            virtual void onAdsReady(Sdk* sdk) = 0;
            virtual void onAdsDidFinish(Sdk* sdk, AdState state) = 0;
            virtual void onBuyFinish(Sdk* sdk, const std::string&  productID, const std::string&  orderID, const std::string&  transID) = 0;
            virtual void onBuyError(Sdk* sdk, const std::string&  productID, const std::string&  orderID, const std::string&  message) = 0;
        };
        class InterDelegate : public cocos2d::Ref{
        public:
            virtual ~InterDelegate() {}
            virtual void showAd() = 0;
            virtual void buy(const char* productID, const char* orderID) = 0;
            virtual void finishTrans(const char* transID) = 0;
            virtual void checkUnFinishTrans() = 0;
        };
    private:
        Delegate *_delegate;
        InterDelegate* _interDelegate;
    public:
        
        Sdk();
        ~Sdk();
        void setInterDelegate(InterDelegate* delegate);
        void setDelegate(Delegate* delegate);
        Delegate* getDelegate();
        void showAd();
        void checkUnFinishTrans();
        void buy(const char* productID, const char* orderID);
        void finishTrans(const char* transID);
        void destroy();
        
        static Sdk* getInstance();
    protected:
        static Sdk* s_SharedSdk;
    };
} 


#endif // __Sdk__CLASS
