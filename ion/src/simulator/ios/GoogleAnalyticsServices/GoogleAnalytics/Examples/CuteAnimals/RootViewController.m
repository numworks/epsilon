//
//  RootViewController.m
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#import "AppDelegate.h"
#import "RootViewController.h"
#import "CategoryViewController.h"
#import "ImageViewController.h"

#import "GAI.h"
#import "GAIDictionaryBuilder.h"

@interface RootViewController ()

@property(nonatomic, strong) NSArray *items;
@property(nonatomic, weak) AppDelegate *delegate;

@end

@implementation RootViewController

- (id)initWithNibName:(NSString *)nibName
               bundle:(NSBundle *)nibBundle {
  self = [super initWithNibName:nibName
                         bundle:nibBundle];
  if (self) {
    _delegate = [UIApplication sharedApplication].delegate;
    self.title = @"Cute Animals";
    self.screenName = @"root";
  }
  return self;
}


- (void)dispatch {
  NSMutableDictionary *event =
      [[GAIDictionaryBuilder createEventWithCategory:@"UI"
                                              action:@"buttonPress"
                                               label:@"dispatch"
                                               value:nil] build];
  [[GAI sharedInstance].defaultTracker send:event];
  [[GAI sharedInstance] dispatch];
}

- (void)crash {
  [NSException raise:@"There is no spoon."
              format:@"Abort, retry, fail?"];
}

- (void)viewDidLoad {
  [super viewDidLoad];
  self.navigationItem.leftBarButtonItem =
      [[UIBarButtonItem alloc] initWithTitle:@"Dispatch"
                                       style:UIBarButtonItemStyleBordered
                                      target:self
                                      action:@selector(dispatch)];
  self.navigationItem.rightBarButtonItem =
      [[UIBarButtonItem alloc] initWithTitle:@"Crash"
                                       style:UIBarButtonItemStyleBordered
                                      target:self
                                      action:@selector(crash)];
  self.items =
      [[self.delegate.images allKeys]
       sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
  [self.tableView reloadData];
}

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

  NSString *category = (self.items)[indexPath.row];
  NSString *nib = ([[UIDevice currentDevice] userInterfaceIdiom] ==
      UIUserInterfaceIdiomPhone) ? @"CategoryViewController_iPhone" :
      @"CategoryViewController_iPad";
  CategoryViewController *categoryController =
      [[CategoryViewController alloc] initWithNibName:nib
                                                bundle:nil
                                              category:category];
  [self.delegate.navController pushViewController:categoryController
                                         animated:YES];
}

#pragma mark UITableViewDataSource methods

- (NSInteger)tableView:(UITableView *)tableView
    numberOfRowsInSection:(NSInteger)section {
  return self.items.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  if (indexPath.row < 0 || indexPath.row >= self.items.count) {
    NSLog(@"IndexPath %d out of bounds!", indexPath.row);
    return nil;
  }
  NSString *category = (self.items)[indexPath.row];

  static NSString *CellId = @"Cell";
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellId];
  if (!cell) {
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle
                                   reuseIdentifier:CellId];
    cell.textLabel.font = [UIFont systemFontOfSize:14];
    cell.detailTextLabel.font = [UIFont systemFontOfSize:12];
  }

  NSString *label = [NSString stringWithFormat:@"Cute %@ Pictures!", category];
  cell.textLabel.text = label;
  NSUInteger imageCount = [(self.delegate.images)[category] count];
  cell.detailTextLabel.text = [NSString stringWithFormat:@"%u image(s).",
                               imageCount];
  return cell;
}

@end
