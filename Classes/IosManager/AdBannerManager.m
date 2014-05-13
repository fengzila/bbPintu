//
//  AdBannerManager.m
//  puzzle
//
//  Created by FengZi on 14-3-3.
//
//

#import "AdBannerManager.h"
#import "AdBanner.h"

@implementation AdBannerManager

+ (void)show {
    
    UIWindow *window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    if (adBannerView) {
        [adBannerView setFrame:CGRectMake([window bounds].size.width/2 - adBannerView.frame.size.width/2, [window bounds].size.height - adBannerView.frame.size.height, adBannerView.frame.size.width, adBannerView.frame.size.height)];
    }
}

+ (void)hiden{
    if (adBannerView) {
        [adBannerView setFrame:CGRectMake(0, -adBannerView.frame.size.height, adBannerView.frame.size.width, adBannerView.frame.size.height)];
    }
}

@end
