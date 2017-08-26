//
//  IAPHelper.h
//  InAppRage
//
//  Created by Ray Wenderlich on 2/28/11.
//  Copyright 2011 Ray Wenderlich. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>


@interface IAPHelper : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    
    //MGChargePlatCallBackDelegate* delegate2;
}


//@property(assign) MGChargePlatCallBackDelegate* delegate2;

+(IAPHelper*) shared;

-(bool) verifyPruchase:(NSData*) receiptData sandBox:(bool) isSandBox;
-(bool) requestProduct:(NSString*)productId;
-(bool) requestProduct:(NSString*)productId request:(SKProductsRequest**) usedRequest;
//-(void) sendLog:(NSString*)content;
-(bool) purchaseProduct:(NSString*)orderId productId:(NSString*)productId quantity:(NSInteger)quantity;
-(void) finishTransaction:(SKPaymentTransaction*)transaction;
-(void) finishTransactionById:(NSString*)tranId;
-(void) restorePurchase;
//-(void) resetIapDelegate;
-(void) checkUnFinishTrans;
-(NSArray*) getUnFinishTrans;
-(void) updateTransactions:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;

@end

/*
#import <Foundation/Foundation.h>
#import "StoreKit/StoreKit.h"

#define kProductsLoadedNotification         @"ProductsLoaded"
#define kProductPurchasedNotification       @"ProductPurchased"
#define kProductPurchaseFailedNotification  @"ProductPurchaseFailed"

@interface IAPHelper : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    NSSet * _productIdentifiers;    
    NSArray * _products;
    NSMutableSet * _purchasedProducts;
    SKProductsRequest * _request;
}

@property (retain) NSSet *productIdentifiers;
@property (retain) NSArray * products;
@property (retain) NSMutableSet *purchasedProducts;
@property (retain) SKProductsRequest *request;

+ (IAPHelper*)sharedInstance;
- (void)requestProduct:(NSString*)productId;

- (void)requestProducts;
- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers;
- (void)buyProductIdentifier:(NSString *)productIdentifier;

@end
*/
