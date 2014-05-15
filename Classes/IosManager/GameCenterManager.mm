//
//  GameCenterManager.m
//  puzzle
//
//  Created by FengZi on 14-2-13.
//
//

#import "GameCenterManager.h"
#include "MobClickCpp.h"
#import <UIKit/UIKit.h>
#include "cocos2d.h"
#include "BBGameDataManager.h"
USING_NS_CC;

@implementation GameCenterManager
@synthesize gameCenterAvailable;
@synthesize isProcessPayments;

//静态初始化 对外接口
static GameCenterManager *instance = nil;

static UIViewController* currentModalViewController = nil;
+ (GameCenterManager*) getInstance {
    if (!instance) {
        instance = [[GameCenterManager alloc] init];
    }
    return instance;
}

//用于验证
- (BOOL)isGameCenterAvailable {
    // check for presence of GKLocalPlayer API
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    
    // check if the device is running iOS 4.1 or later
    NSString *reqSysVer =@"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer
                                           options:NSNumericSearch] != NSOrderedAscending);
    
    return (gcClass && osVersionSupported);
}

- (id)init {
    if ((self = [super init])) {
        gameCenterAvailable = [self isGameCenterAvailable];
        if (gameCenterAvailable) {
            NSNotificationCenter *nc =
            [NSNotificationCenter defaultCenter];
            [nc addObserver:self
                   selector:@selector(authenticationChanged)
                       name:GKPlayerAuthenticationDidChangeNotificationName
                     object:nil];
        }
    }
    return self;
}

//后台回调登陆验证
- (void)authenticationChanged {
    
    if ([GKLocalPlayer localPlayer].isAuthenticated &&!userAuthenticated) {
        NSLog(@"Authentication changed: player authenticated.");
        userAuthenticated = TRUE;
    } else if (![GKLocalPlayer localPlayer].isAuthenticated && userAuthenticated) {
        NSLog(@"Authentication changed: player not authenticated");
        userAuthenticated = FALSE;
    }
    
}

- (void)authenticateLocalUser {
    
    if (!gameCenterAvailable) return;
    
    NSLog(@"Authenticating local user...");
    if([GKLocalPlayer localPlayer].authenticated == NO){
        [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error){
            NSLog(@"Reason: %@", [error localizedDescription]);
            [self callDelegateOnMainThread: @selector(processGameCenterAuth:) withArg: NULL error: error];
        }];
    }
	else {
	}
}

// 认证失败
- (void) callDelegateOnMainThread: (SEL) selector withArg: (id) arg error: (NSError*) err
{
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [self callDelegate: selector withArg: arg error: err];
    });
}

- (void) callDelegate: (SEL) selector withArg: (id) arg error: (NSError*) err
{
    assert([NSThread isMainThread]);
    if([_delegate respondsToSelector: selector])
    {
        if(arg != NULL){
            [_delegate performSelector: selector withObject: arg withObject: err];
        }else{
            [_delegate performSelector: selector withObject: err];
        }
    }else{
        NSLog(@"Missed Method");
    }
}

// 处理gamecenter认证结果
- (void) processGameCenterAuth: (NSError*) error
{
    if(error == NULL){
    }else{
		UIAlertView* alert= [[[UIAlertView alloc] initWithTitle: @"Game Center Account Required"
                                                        message: [NSString stringWithFormat: @"Reason: %@", [error localizedDescription]]
                                                       delegate: self cancelButtonTitle: @"Try Again…" otherButtonTitles: NULL] autorelease];
        [alert show];
	}
}

- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController{}

- (void)matchmakerViewControllerWasCancelled:(GKMatchmakerViewController *)viewController{}

- (void)matchmakerViewController:(GKMatchmakerViewController *)viewController didFailWithError:(NSError *)error{}

//显示排行榜
- (void) showLeaderboard
{
    if (!gameCenterAvailable) return;
    
    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
    if (leaderboardController != nil) {
        leaderboardController.leaderboardDelegate = self;
        
        UIWindow *window = [[UIApplication sharedApplication] keyWindow];
        currentModalViewController = [[UIViewController alloc] init];
        [window addSubview:currentModalViewController.view];
        [currentModalViewController presentModalViewController:leaderboardController animated:YES];
    }
}

//- (void) showLeaderboard
//{
//    UIViewController *tempVC=[[UIViewController alloc] init];
//    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
//    if (leaderboardController != nil)
//    {
//        UIApplication* clientApp = [UIApplication sharedApplication];
//        UIWindow* topWindow = [clientApp keyWindow];
//        if (!topWindow)
//        {
//            topWindow = [[clientApp windows] objectAtIndex:0];
//        }
//        [topWindow addSubview:tempVC.view];
//        leaderboardController.leaderboardDelegate = self;
//        [tempVC presentModalViewController: leaderboardController animated: YES];
//    }
//}

//关闭排行榜回调
- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController{
    if(currentModalViewController !=nil){
        [currentModalViewController dismissModalViewControllerAnimated:NO];
        [currentModalViewController release];
        [currentModalViewController.view removeFromSuperview];
        currentModalViewController = nil;
    }
}

// 上传分数
- (void) reportScore: (int64_t) score forCategory: (NSString*) category
{
    GKScore *scoreReporter = [[[GKScore alloc] initWithCategory:category] autorelease];
    scoreReporter.value = score;
    
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
        if (error != nil)
        {
            // handle the reporting error
            NSLog(@"上传分数出错.");
            //If your application receives a network error, you should not discard the score.
            //Instead, store the score object and attempt to report the player’s process at
            //a later time.
        }else {
            NSLog(@"上传分数成功");
        }
        
    }];
}

- (void)match:(GKMatch *)match didReceiveData:(NSData *)data fromPlayer:(NSString *)playerID
{
    
}

+ (void)goToAppStoreEvaluate
{
    NSURL *url;
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 7.0)
    {
        url = [NSURL URLWithString:@"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=876313246"];
    }
    else
    {
        url = [NSURL URLWithString:@"itms-apps://itunes.apple.com/app/id876313246?at=10l6dK"];
    }
    [[UIApplication sharedApplication] openURL:url];
}



//******************** IAP ******************************//
// 初始化内消费
- (void) initStoreKit
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

- (BOOL) canProcessPayments
{
    if ([SKPaymentQueue canMakePayments]) {
        return YES;
    } else {
        return NO;
    }
}

// 购买产品, 发送购买请求
- (void) purchaseItem: (NSString*)identifier
{
    if (![self canProcessPayments]) {
        NSLog(@"canProcessPayments: NO");
        return;
    }
    
    // 使用请求商品信息式购买
    SKProductsRequest *request= [[SKProductsRequest alloc]
                                 initWithProductIdentifiers: [NSSet setWithObject: identifier]];
    request.delegate = self;
    [request start];
    
// 直接购买对应商品，不推荐
//    isProcessPayments = YES;
//    SKPayment * payment = [SKPayment paymentWithProductIdentifier:identifier];
//    [[SKPaymentQueue defaultQueue] addPayment:payment];
    
}

// SKProductsRequest 的回调
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *myProduct = response.products;
    
    if (myProduct.count == 0) {
        NSLog(@"无法获取产品信息，购买失败。");
        
        NSLog(@"无效的产品product ID = %@",response.invalidProductIdentifiers);
        
        isProcessPayments = NO;
        
        MobClickCpp::event("Buy_Hint_Cancel");
        //    NotificationCenter::getInstance()->postNotification("kIapCancel");
        NotificationCenter::getInstance()->postNotification("kRefreshUI");
        return;
    }
    NSLog(@"获取产品信息-->OK");
    isProcessPayments = YES;
    SKPayment * payment = [SKPayment paymentWithProduct:myProduct[0]];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
    [request autorelease];
}

// SKPayment 的回调
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
                break;
                
            default:
                break;
        }
    }
}

// 结束交易
- (void) completeTransaction: (SKPaymentTransaction*)transaction
{
    isProcessPayments = NO;
	
	//application should implement these two methods.
    [self recordTransaction: transaction];
    [self provideContent: transaction.payment.productIdentifier];
	
    // Remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

// 重置交易
- (void) restoreTransaction: (SKPaymentTransaction*)transaction
{
    isProcessPayments = NO;
	
    [self recordTransaction: transaction];
    [self provideContent: transaction.originalTransaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

// 交易失败
- (void) failedTransaction: (SKPaymentTransaction*)transaction
{
    isProcessPayments = NO;
    if (transaction.error.code == SKErrorPaymentCancelled)
    {
        
	}
	else{
        
	}
    NSLog(@"failedTransaction error.code:%d",transaction.error.code);
	
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    MobClickCpp::event("Buy_Hint_Cancel");
//    NotificationCenter::getInstance()->postNotification("kIapCancel");
    NotificationCenter::getInstance()->postNotification("kRefreshUI");
    
    NSString* strMsg = @"购买失败，请稍后再试。";
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"金手指*3" message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
    [alert release];
    
    MobClickCpp::event("Buy_Hint_Success");
    NotificationCenter::getInstance()->postNotification("kRefreshUI");
}

// 交易记录
- (void) recordTransaction: (SKPaymentTransaction*)transacation
{
    
}

// 提供产品
- (void) provideContent: (NSString*)identifier
{
    NSString* strMsg;
    if ([identifier compare:@"goldFingerX3"] == 0) {
        strMsg = @"你已经成功购买三个金手指，纵情享受它吧！";
        BBGameDataManager::getInstance()->addCanUseTipsCount();
    }
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"金手指*3" message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
    [alert release];
    
    MobClickCpp::event("Buy_Hint_Success");
    NotificationCenter::getInstance()->postNotification("kRefreshUI");
//    NotificationCenter::getInstance()->postNotification("kIapCancel");
}


@end

