//
//  ViewController.h
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "NavController.h"

@interface CategoryViewController :
    GAITrackedViewController<UITableViewDelegate, UITableViewDataSource>

@property(nonatomic, strong) NSString *category;
@property(nonatomic, strong) IBOutlet UITableView *tableView;

- (id)initWithNibName:(NSString *)nibName
               bundle:(NSBundle *)nibBundle
             category:(NSString *)category;

@end
