//
//  IAPHelper.m
//  InAppRage
//
//  Created by Ray Wenderlich on 2/28/11.
//  Copyright 2011 Ray Wenderlich. All rights reserved.
//


#import "IAPHelper.h"
#import "Sdk.h"
#import "custom/sdk/Sdk.h"

#include <string>
#include <map>
//using namespace std;

//static NSMutableDictionary productQueryList;


/*
struct ProductPurchaseData
{
    std::string data;
    std::string transactionIdentifier;
    std::string productIdentifier;
    std::string transactionReceipt;
    
};
*/
struct BuyInfo {
    std::string productID;
    std::string orderID;
};

static std::map<id,BuyInfo> buyList;

//static std::map<id, ProductPurchaseData> productQueryList;

@implementation IAPHelper

//@synthesize delegate;
//@synthesize delegate2;

+(IAPHelper*) shared
{
    static IAPHelper *instance;
    @synchronized(self) {
        if(!instance) {
            instance = [[IAPHelper alloc] init];
            
            // Assign an observer to monitor the transaction status.
            [[SKPaymentQueue defaultQueue] addTransactionObserver:instance];
            //[instance resetIapDelegate];
            
            //MGCharge::shared()->setCharegPlat(self);
            //productQueryList = [NSMutableDictionary dictionary];

            
        }
    }
    return instance;
}


-(bool) purchaseProduct:(NSString*)orderId productId:(NSString*)productId quantity:(NSInteger)quantity
{
    if (productId != nil) {
        
        NSLog(@"==========purchaseProduct: %@", productId);
        
        if ([SKPaymentQueue canMakePayments]) {
            
            SKProductsRequest* usedrequest;
            if([self requestProduct: productId request:&usedrequest])
            {
                BuyInfo info;
                info.orderID = [orderId UTF8String];
                info.productID = [productId UTF8String];
                buyList[usedrequest] = info;
            }
            
            
            /*
            SKMutablePayment *paymentRequest = [SKMutablePayment paymentWithProductIdentifier:productId];
            //SKPayment *payment = [SKPayment paymentWithProductIdentifier:productId];
            
            paymentRequest.quantity = quantity;
            paymentRequest.requestData = [orderId dataUsingEncoding: NSUTF8StringEncoding];
            // Request a purchase of the product.
            [[SKPaymentQueue defaultQueue] addPayment:paymentRequest];
            */
            return YES;
            
        } else {
            NSLog(@"1==========purchaseProduct IAP Disabled");
            
            Sdk::getInstance()->getDelegate()->onBuyError(Sdk::getInstance(), [productId UTF8String], [orderId UTF8String], "请先开启应用内支付");
            
            return NO;
        }
        
    } else {
        NSLog(@"purchaseProduct: productId = NIL");
        return NO;
    }
}


-(bool) requestProduct:(NSString*)productId
{
    return [self requestProduct:productId request:nil];
}

-(bool) requestProduct:(NSString*)productId request:(SKProductsRequest**) usedRequest
{
    
    if (productId != nil) {
        
        NSLog(@"requestProduct: %@", productId);
        
        if ([SKPaymentQueue canMakePayments]) {
            // Yes, In-App Purchase is enabled on this device.
            // Proceed to fetch available In-App Purchase items.
            
            // Initiate a product request of the Product ID.
            SKProductsRequest *prodRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productId]];
            prodRequest.delegate = self;
            [prodRequest start];
            [prodRequest release];
            
            if(usedRequest != nil)
            {
                *usedRequest =  prodRequest;
            }
            
            return YES;
            
        } else {
            // Notify user that In-App Purchase is Disabled.
            NSLog(@"requestProduct: IAP Disabled");
            
            return NO;
        }
        
    } else {
        
        NSLog(@"requestProduct: productId = NIL");
        
        return NO;
    }
    
    
}



#pragma mark -
#pragma mark SKProductsRequestDelegate Methods

// Store Kit returns a response from an SKProductsRequest.
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    std::map<id, BuyInfo>::iterator iter = buyList.find(request);
    
    if(iter != buyList.end())
    {
        BuyInfo& info = (*iter).second;
        
        NSArray *products = response.products;
        SKProduct *product = [products count] > 0 ? [products objectAtIndex:0] : nil;
        if (product) {
            SKMutablePayment *paymentRequest = [SKMutablePayment paymentWithProduct:product];
            
            paymentRequest.quantity = 1;
            NSString* orderId = [NSString stringWithUTF8String:info.orderID.c_str()];
            paymentRequest.requestData = [orderId dataUsingEncoding: NSUTF8StringEncoding];
            // Request a purchase of the product.
            [[SKPaymentQueue defaultQueue] addPayment:paymentRequest];
        } else {
            //无法获取商品信息
            Sdk::getInstance()->getDelegate()->onBuyError(Sdk::getInstance(),info.productID.c_str(), info.orderID.c_str(), "无法获取商品信息");
            
            NSLog(@"无法获取商品信息");
        }
        buyList.erase(iter);
    }
    
    
    /*
    // Parse the received product info.
    SKProduct *validProduct = nil;
    int count = [response.products count];
    NSLog(@"============product count: %d" , count);
    for(int i = 0; i < count; i++)
    {
        // Grab the first product in the array.
        validProduct = [response.products objectAtIndex:i];
        
        std::map<id, ProductPurchaseData>::iterator iter = productQueryList.find(request);
        
        if(iter != productQueryList.end())
        {
            ProductPurchaseData& purchaseData = (*iter).second;
            if(Sdk::getInstance()->getDelegate()){
                //TODO 验证
                Sdk::getInstance()->getDelegate()->onBuyFinish(Sdk::getInstance(), purchaseData.productIdentifier.c_str(), purchaseData.data.c_str(), purchaseData.transactionIdentifier.c_str());
                
                productQueryList.erase(iter);
            }
        }
        
        // Yes, product is available, so return values.
        
        NSLog(@"======================%@,%@,%@,%@,%@",validProduct.productIdentifier
              , validProduct.localizedTitle
              , validProduct.price
              , validProduct.localizedDescription
              , validProduct.priceLocale.localeIdentifier
              );
    }
    */
}


#pragma mark -
#pragma mark SKPaymentTransactionObserver Methods


-(void)updateTransactions:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    NSLog(@"1==========updateTransactions:%lu", [transactions count]);
    
    for(SKPaymentTransaction *transaction in transactions) {
        
        
        NSString* temp  =  [[NSString alloc] initWithData:transaction.payment.requestData encoding:NSUTF8StringEncoding];
        const char* orderID =  [temp UTF8String];
        [temp release];
        
        NSData *receipt = nil;
        if ([[NSBundle mainBundle] respondsToSelector:@selector(appStoreReceiptURL)]) {
            NSURL *receiptUrl = [[NSBundle mainBundle] appStoreReceiptURL];
            receipt = [NSData dataWithContentsOfURL:receiptUrl];
            NSLog(@"---------- new get transactionReceipt");
        } else {
            if ([transaction respondsToSelector:@selector(transactionReceipt)]) {
                //Works in iOS3 - iOS8, deprected since iOS7, actual deprecated (returns nil) since iOS9
                receipt = [transaction transactionReceipt];
                NSLog(@"---------- old get transactionReceipt");
            }
        }
        
        //temp = [[NSString alloc] initWithData:transaction.transactionReceipt encoding:NSUTF8StringEncoding];
        /*
        temp =[[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding];
        const char* transactionReceipt =[temp UTF8String];
        [temp release];
        */
        
        const char* productID = [transaction.payment.productIdentifier UTF8String];
        
        NSLog(@"transaction.transactionState=%ld", (long)transaction.transactionState);
        
        switch (transaction.transactionState) {
                
            case SKPaymentTransactionStatePurchasing:
                // Item is still in the process of being purchased
                
                break;
                
            case SKPaymentTransactionStatePurchased:
            {
                const char* transactionIdentifier = [transaction.transactionIdentifier UTF8String];
                
                [self verifyPruchase:receipt sandBox:false];
                
                if(Sdk::getInstance()->getDelegate()){
                    // 验证
                    if([self verifyPruchase:receipt sandBox:false]){
                        Sdk::getInstance()->getDelegate()->onBuyFinish(Sdk::getInstance(), productID, orderID, transactionIdentifier);
                    }else{
                        Sdk::getInstance()->getDelegate()->onBuyError(Sdk::getInstance(), productID, orderID, "验证失败!");
                    }
                }
                
                break;
            }
                
            case SKPaymentTransactionStateRestored:
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                break;
                
            case SKPaymentTransactionStateFailed:
                // Purchase was either cancelled by user or an error occurred.
                
                if (transaction.error.code == SKErrorUnknown)
                {
                    NSLog(@"err 0:%@",transaction.error.localizedDescription);
                }
                else if (transaction.error.code != SKErrorPaymentCancelled)
                {
                    // A transaction error occurred, so notify user.
                    NSLog(@"err 1:%@",transaction.error.localizedDescription);
                }
                else
                {
                    NSLog(@"err 2:%@",transaction.error.localizedDescription);
                }
                // Finished transactions should be removed from the payment queue.
                
                Sdk::getInstance()->getDelegate()->onBuyError(Sdk::getInstance(), productID, orderID, [transaction.error.localizedDescription UTF8String]);
                
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                break;
            case SKPaymentTransactionStateDeferred:
                break;
        }
        
    }
    
}


-(void) finishTransaction:(SKPaymentTransaction*)transaction{
    
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

-(void) finishTransactionById:(NSString *)tranId
{
    NSArray *transactions = [[SKPaymentQueue defaultQueue] transactions];
    for(SKPaymentTransaction *transaction in transactions)
    {
        if(NSOrderedSame == [transaction.transactionIdentifier compare:tranId options:NSCaseInsensitiveSearch])
        {
            [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
            break;
        }
    }
}


//2.本地验证
- (bool)verifyPruchase:(NSData *) receiptData sandBox:(bool) isSandBox{
    /*
    // 验证凭据，获取到苹果返回的交易凭据
    // appStoreReceiptURL iOS7.0增加的，购买交易完成后，会将凭据存放在该地址
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    // 从沙盒中获取到购买凭据
    NSData *receiptData = [NSDatadataWithContentsOfURL:receiptURL];
    */
    // 发送网络POST请求，对购买凭据进行验证
    NSURL *url;
    if(isSandBox){
        url = [NSURL URLWithString:@"https://sandbox.itunes.apple.com/verifyReceipt"];
    }else{
        url = [NSURL URLWithString:@"https://buy.itunes.apple.com/verifyReceipt"];
    }
    // 国内访问苹果服务器比较慢，timeoutInterval需要长一点
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10.0f];
    
    request.HTTPMethod =@"POST";
    
    // 在网络中传输数据，大多情况下是传输的字符串而不是二进制数据
    // 传输的是BASE64编码的字符串
    /**
     BASE64 常用的编码方案，通常用于数据传输，以及加密算法的基础算法，传输过程中能够保证数据传输的稳定性
     BASE64是可以编码和解码的
     */
    NSString *encodeStr = [receiptData                                     base64EncodedStringWithOptions:NSDataBase64EncodingEndLineWithLineFeed];
    
    NSString *payload = [NSString stringWithFormat:@"{\"receipt-data\" : \"%@\"}", encodeStr];
    NSData *payloadData = [payload dataUsingEncoding:NSUTF8StringEncoding];
    
    request.HTTPBody = payloadData;
    
    // 提交验证请求，并获得官方的验证JSON结果
    NSData *result = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
    
    // 官方验证结果为空
    if (result ==nil) {
        NSLog(@"验证失败");
        return false;
    }
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:result options:NSJSONReadingAllowFragments error:nil];
    
    NSLog(@"%@", dict);
    
    if (dict !=nil) {
        int status = [[dict objectForKey:@"status"] intValue];
        if(status == 21007){
            //eceipt是Sandbox receipt，但却发送至生产系统的验证服务
            return [self verifyPruchase:receiptData sandBox:true];
        } else if(status == 21008){
            //receipt是生产receipt，但却发送至Sandbox环境的验证服务
            return [self verifyPruchase:receiptData sandBox:false];
        } else if(status == 0){
            // 比对字典中以下信息基本上可以保证数据安全
            // bundle_id&application_version&product_id&transaction_id
            NSString* bundleID = [[dict objectForKey:@"receipt"] objectForKey:@"bundle_id"];
            NSLog(@"bundle_id:%@", [[dict objectForKey:@"receipt"] objectForKey:@"bundle_id"]);
            NSLog(@"===%@",[[NSBundle mainBundle] bundleIdentifier]);
            if([[[NSBundle mainBundle] bundleIdentifier] isEqualToString:bundleID]){
                NSLog(@"验证成功");
                return true;
            }
            return false;
        }else{
            return false;
        }
    }
    return false;
}



-(void) checkUnFinishTrans
{
    [self updateTransactions:[SKPaymentQueue defaultQueue] updatedTransactions:[[SKPaymentQueue defaultQueue] transactions]];
}

-(NSArray*) getUnFinishTrans
{
    return [[SKPaymentQueue defaultQueue] transactions];
}

-(void) sendLog:(NSString*)content
{
    /*
     NSDate *date = [NSDate date];
     NSTimeInterval sec = [date timeIntervalSince1970];
     NSDate *epochNSDate = [[NSDate alloc] initWithTimeIntervalSince1970:sec];
     NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
     [dateFormatter setDateFormat:@"yyyy-MM-dd_HH:mm:ss"];
     NSString *na =[dateFormatter stringFromDate:epochNSDate];
     
     //显示时区//
     NSString *currentTimeZone = [[dateFormatter timeZone] abbreviation];
     NSLog (@"(Your local time zone is: %@)", currentTimeZone);
     
     NSLog(@"currentTime=%@",na);
     */
    
}

-(void) restorePurchase
{
    NSLog(@"restorePurchase");
    
    if ([SKPaymentQueue canMakePayments]) {
        // Yes, In-App Purchase is enabled on this device.
        // Proceed to restore purchases.
        
        
        // Request to restore previous purchases.
        [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
        
        return ;
        
    } else {
        // Notify user that In-App Purchase is Disabled.
        return ;
    }
}





// The transaction status of the SKPaymentQueue is sent here.
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions {
    NSLog(@"=============paymentQueue");
    [self updateTransactions:queue updatedTransactions:transactions];
}

// Called when one or more transactions have been removed from the queue.
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
{
    NSLog(@"=========removedTransactions");
    
    // Release the transaction observer since transaction is finished/removed.
    //[[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

// Called when SKPaymentQueue has finished sending restored transactions.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    
    NSLog(@"paymentQueueRestoreCompletedTransactionsFinished");
    
    if ([queue.transactions count] == 0) {
        // Queue does not include any transactions, so either user has not yet made a purchase
        // or the user's prior purchase is unavailable, so notify app (and user) accordingly.
        
        NSLog(@"EBPurchase restore queue.transactions count == 0");
        
        // Release the transaction observer since no prior transactions were found.
        //[[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
        
        //[delegate incompleteRestore:self];
        
    } else {
        // Queue does contain one or more transactions, so return transaction data.
        // App should provide user with purchased product.
        
        NSLog(@"EBPurchase restore queue.transactions available");
        
        for(SKPaymentTransaction *transaction in queue.transactions) {
            
            NSLog(@"EBPurchase restore queue.transactions - transaction data found");
            
            //[delegate successfulPurchase:self identifier:transaction.payment.productIdentifier receipt:transaction.transactionReceipt];
        }
    }
}



// Called if an error occurred while restoring transactions.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    // Restore was cancelled or an error occurred, so notify user.
    
    NSLog(@"EBPurchase restoreCompletedTransactionsFailedWithError");
    
    //[delegate failedRestore:self error:error.code message:error.localizedDescription];
}


#pragma mark - Internal Methods & Events

- (void)dealloc
{
    //[validProduct release];
    
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [super dealloc];
}

@end


/*
#import "IAPHelper.h"

@implementation IAPHelper





@synthesize productIdentifiers = _productIdentifiers;
@synthesize products = _products;
@synthesize purchasedProducts = _purchasedProducts;
@synthesize request = _request;


+ (instancetype)sharedInstance {
    static IAPHelper* instance = nil;
    static dispatch_once_t onceToken = 0;
    dispatch_once(&onceToken, ^{
        instance = [[IAPHelper alloc] init];
    });
    return instance;
}

#pragma mark - 请求商品信息
- (void)requestProduct:(NSString*)productId {
 
    NSSet *productIds = [NSSet setWithObject:productId];
    SKProductsRequest *productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIds];
    productsRequest.delegate = self;
    [productsRequest start];
}

#pragma mark - appstore回调 请求商品信息回调
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    NSArray *products = response.products;
    SKProduct *product = [products count] > 0 ? [products objectAtIndex:0] : nil;
    if (product) {
        //添加付款请求到队列
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    } else {
        //无法获取商品信息
        [self.appStoreDelegate sdkAppStoreNoProductInfo];
        SDKLog(@"无法获取商品信息");
    }
}

- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers {
    if ((self = [super init])) {
        
        // Store product identifiers
        _productIdentifiers = [productIdentifiers retain];
        
        // Check for previously purchased products
        NSMutableSet * purchasedProducts = [NSMutableSet set];
        for (NSString * productIdentifier in _productIdentifiers) {
            BOOL productPurchased = [[NSUserDefaults standardUserDefaults] boolForKey:productIdentifier];
            if (productPurchased) {
                [purchasedProducts addObject:productIdentifier];
                NSLog(@"Previously purchased: %@", productIdentifier);
            }
            NSLog(@"Not purchased: %@", productIdentifier);
        }
        self.purchasedProducts = purchasedProducts;
                        
    }
    return self;
}

- (void)requestProducts {
    
    self.request = [[[SKProductsRequest alloc] initWithProductIdentifiers:_productIdentifiers] autorelease];
    _request.delegate = self;
    [_request start];
    
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    NSLog(@"Received products results...");   
    self.products = response.products;
    self.request = nil;    
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kProductsLoadedNotification object:_products];    
}

- (void)recordTransaction:(SKPaymentTransaction *)transaction {    
    // TODO: Record the transaction on the server side...    
}

- (void)provideContent:(NSString *)productIdentifier {
    
    NSLog(@"Toggling flag for: %@", productIdentifier);
    [[NSUserDefaults standardUserDefaults] setBool:TRUE forKey:productIdentifier];
    [[NSUserDefaults standardUserDefaults] synchronize];
    [_purchasedProducts addObject:productIdentifier];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kProductPurchasedNotification object:productIdentifier];
    
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
    
    NSLog(@"completeTransaction...");
    
    [self recordTransaction: transaction];
    [self provideContent: transaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
    
    NSLog(@"restoreTransaction...");
    
    [self recordTransaction: transaction];
    [self provideContent: transaction.originalTransaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        NSLog(@"Transaction error: %@", transaction.error.localizedDescription);
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kProductPurchaseFailedNotification object:transaction];
    
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
            default:
                break;
        }
    }
}

- (void)buyProductIdentifier:(NSString *)productIdentifier {
    
    NSLog(@"Buying %@...", productIdentifier);
    
    SKPayment *payment = [SKPayment paymentWithProductIdentifier:productIdentifier];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
    
}

- (void)dealloc
{
    [_productIdentifiers release];
    _productIdentifiers = nil;
    [_products release];
    _products = nil;
    [_purchasedProducts release];
    _purchasedProducts = nil;
    [_request release];
    _request = nil;
    [super dealloc];
}

@end

*/
