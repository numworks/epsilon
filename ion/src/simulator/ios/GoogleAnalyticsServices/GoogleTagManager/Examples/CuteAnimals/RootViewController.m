//
//  RootViewController.m
//  CuteAnimals
//
//  Copyright 2013 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "AppDelegate.h"
#import "RootViewController.h"
#import "CategoryViewController.h"
#import "ImageViewController.h"
#import "TAGContainer.h"
#import "TAGDataLayer.h"
#import "TAGManager.h"
#import "Utils.h"

@interface RootViewController ()

@property(nonatomic, retain) NSArray *items;
@property(nonatomic, assign) AppDelegate *delegate;

@end

@implementation RootViewController

@synthesize tableView = _tableView;
@synthesize navController = _navController;
@synthesize items = _items;
@synthesize delegate = _delegate;

- (id)initWithNibName:(NSString *)nibName
               bundle:(NSBundle *)nibBundle {
  self = [super initWithNibName:nibName
                         bundle:nibBundle];
  if (self) {
    self.delegate = [UIApplication sharedApplication].delegate;
    self.title = [self.delegate.container stringForKey:@"Title"];
  }
  return self;
}

- (void)refresh {
  [self.delegate.container refresh];
  // Push a "refresh" event. Tags that match that event will fire.
  [self.delegate.tagManager.dataLayer pushValue:@"refresh" forKey:@"event"];
  // Push the "custom tag" event to trigger firing a custom function call tag.
  [self.delegate.tagManager.dataLayer pushValue:@"custom_tag" forKey:@"event"];
}

// Override
- (void)viewDidLoad {
  [super viewDidLoad];
  self.navigationItem.rightBarButtonItem =
      [[UIBarButtonItem alloc] initWithTitle:@"Refresh"
                                       style:UIBarButtonItemStyleBordered
                                      target:self
                                      action:@selector(refresh)];
  self.items =
      [[self.delegate.images allKeys]
       sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
  [self.tableView reloadData];
}

// Override
- (void)viewDidAppear:(BOOL)animated {
  [super viewDidAppear:animated];
  [Utils pushOpenScreenEventWithScreenName:@"MainScreen"];
}

// Override
- (void)viewWillDisappear:(BOOL)animated {
  [super viewDidDisappear:animated];
  [Utils pushCloseScreenEventWithScreenName:@"MainScreen"];
}

// Override
- (BOOL)shouldAutorotateToInterfaceOrientation:
    (UIInterfaceOrientation)interfaceOrientation {
  return NO;
}

#pragma mark UITableViewDelegate methods

- (void)tableView:(UITableView *)tableView
    didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  [tableView deselectRowAtIndexPath:indexPath animated:NO];
  if (indexPath.row < 0 || indexPath.row >= self.items.count) {
    NSLog(@"IndexPath %d out of bounds!", indexPath.row);
    return;
  }

  NSString *category = [self.items objectAtIndex:indexPath.row];
  NSString *nib =
      ([[UIDevice currentDevice] userInterfaceIdiom] ==
          UIUserInterfaceIdiomPhone) ?
          @"CategoryViewController_iPhone" : @"CategoryViewController_iPad";
  CategoryViewController *categoryController =
      [[CategoryViewController alloc] initWithNibName:nib
                                               bundle:nil
                                             category:category];
  categoryController.navController = self.navController;
  [self.navController pushViewController:categoryController animated:YES];
}

#pragma mark UITableViewDataSource methods

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return self.items.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  if (indexPath.row < 0 || indexPath.row >= self.items.count) {
    NSLog(@"IndexPath %d out of bounds!", indexPath.row);
    return nil;
  }
  NSString *category = [self.items objectAtIndex:indexPath.row];

  static NSString *CellId = @"Cell";
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellId];
  if (!cell) {
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle
                                  reuseIdentifier:CellId];
  }

  NSString *adjective = [self.delegate.container stringForKey:@"Adjective"];
  NSString *label = [NSString stringWithFormat:@"%@ %@ Pictures!", adjective, category];
  cell.textLabel.text = label;
  cell.textLabel.font = [UIFont systemFontOfSize:14];
  NSUInteger imageCount = [(NSArray *)[self.delegate.images objectForKey:category] count];
  cell.detailTextLabel.text = [NSString stringWithFormat:@"%u image(s).", imageCount];
  cell.detailTextLabel.font = [UIFont systemFontOfSize:12];
  return cell;
}

@end
