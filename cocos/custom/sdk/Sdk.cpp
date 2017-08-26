// Sdk.cpp
#include "Sdk.h"


Sdk* Sdk::s_SharedSdk = nullptr;

Sdk* Sdk::getInstance()
{
    if (!s_SharedSdk)
    {
        s_SharedSdk = new (std::nothrow) Sdk();
        CCASSERT(s_SharedSdk, "FATAL: Not enough memory");
    }
    
    return s_SharedSdk;
}


Sdk::Sdk(){
    _delegate = nullptr;
    _interDelegate = nullptr;
}

Sdk::~Sdk(){
    if(_delegate!=nullptr)
        delete _delegate;
    if(_interDelegate!=nullptr)
        delete _interDelegate;
    
    CC_ASSERT(this == s_SharedSdk);
    s_SharedSdk = nullptr;
}

void Sdk::setDelegate(Delegate* delegate){
    
    if(_delegate!=nullptr)
        _delegate->release();
    _delegate = delegate;
    _delegate->retain();
    
    //_delegate = const_cast<Delegate*>(&delegate);
}
Sdk::Delegate* Sdk::getDelegate(){
    return _delegate;
}
void Sdk::setInterDelegate(InterDelegate* delegate){
    
    if(_interDelegate!=nullptr)
        _interDelegate->release();
    _interDelegate = delegate;
    _interDelegate->retain();
    
    //_delegate = const_cast<Delegate*>(&delegate);
}
void Sdk::buy(const char* productID, const char* orderID){
    if(_interDelegate!=nullptr){
        _interDelegate->buy(productID, orderID);
    }else{
        if(_delegate!=nullptr){
            _delegate->onBuyError(this, productID, orderID, "interDelegate未初始化!");
        }
    }
}

void Sdk::finishTrans(const char* transID){
    if(_interDelegate!=nullptr){
        _interDelegate->finishTrans(transID);
    }
}

void Sdk::checkUnFinishTrans(){
    if(_interDelegate!=nullptr){
        _interDelegate->checkUnFinishTrans();
    }
}

void Sdk::showAd(){
    if(_interDelegate!=nullptr){
        _interDelegate->showAd();
    }
}
void Sdk::destroy(){
    delete this;
}
