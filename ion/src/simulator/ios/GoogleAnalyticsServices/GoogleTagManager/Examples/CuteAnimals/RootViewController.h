//
//  RootViewController.h
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import <UIKit/UIKit.h>

@interface RootViewController :
    UIViewController<UITableViewDelegate, UITableViewDataSource>

@property(nonatomic, retain) NavController *navController;
@property(nonatomic, retain) IBOutlet UITableView *tableView;

@end
