//
//  GameCenterManager.h
//  puzzle
//
//  Created by FengZi on 14-2-13.
//
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>
#import <StoreKit/StoreKit.h>

@protocol GameCenterManagerDelegate <NSObject>
@optional
- (void) processGameCenterAuth : (NSString*)error;
@end

@interface GameCenterManager :  NSObject <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GKMatchmakerViewControllerDelegate, GKMatchDelegate, SKPaymentTransactionObserver, SKProductsRequestDelegate>{
    
    BOOL userAuthenticated;
    
    // 是否正在购买
    BOOL isProcessPayments;
}

@property (nonatomic, assign) id <GameCenterManagerDelegate> delegate;
@property (assign, readonly) BOOL gameCenterAvailable;
@property (nonatomic) BOOL isProcessPayments;

+ (GameCenterManager *)getInstance;
- (void) authenticateLocalUser;

- (void) showLeaderboard;
- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController;
- (void) reportScore: (int64_t) score forCategory: (NSString*) category;
+ (void)goToAppStoreEvaluate;

// 初始化内消费
- (void) initStoreKit;

// 是否可以内消费
- (BOOL) canProcessPayments;

// 购买产品
- (void) purchaseItem: (NSString*)identifier;

// 结束交易
- (void) completeTransaction: (SKPaymentTransaction*)transaction;

// 重置交易
- (void) restoreTransaction: (SKPaymentTransaction*)transaction;

// 交易失败
- (void) failedTransaction: (SKPaymentTransaction*)transaction;

// 交易记录
- (void) recordTransaction: (SKPaymentTransaction*)transacation;

// 提供产品
- (void) provideContent: (NSString*)identifier;

@end