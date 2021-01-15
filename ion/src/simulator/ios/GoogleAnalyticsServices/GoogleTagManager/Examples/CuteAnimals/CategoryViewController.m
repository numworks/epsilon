//
//  CategoryViewController.m
//  CuteAnimals
//
//  Copyright 2012 Google, Inc. All rights reserved.
//

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

#import "AppDelegate.h"
#import "CategoryViewController.h"
#import "ImageViewController.h"
#import "TAGContainer.h"
#import "TAGDataLayer.h"
#import "TAGManager.h"
#import "Utils.h"

@interface CategoryViewController ()

@property(nonatomic, retain) NSMutableArray *items;
@property(nonatomic, assign) AppDelegate *delegate;

@end

@implementation CategoryViewController

@synthesize category = _category;
@synthesize navController = _navController;
@synthesize tableView = _tableView;
@synthesize items = _items;
@synthesize delegate = _delegate;

- (id)initWithNibName:(NSString *)nibName
               bundle:(NSBundle *)nibBundle
             category:(NSString *)category {
  self = [super initWithNibName:nibName
                         bundle:nibBundle];
  if (self) {
    self.category = category;
    self.delegate = [UIApplication sharedApplication].delegate;
    NSString *adjective = [self.delegate.container stringForKey:@"Adjective"];
    self.title = [NSString stringWithFormat:@"%@ %@s", adjective, category];
  }
  return self;
}

// Override
- (void)viewDidLoad {
  [super viewDidLoad];
  self.items = [self.delegate.images objectForKey:self.category];
  // Put the category_name into the data layer for future use.
  [self.delegate.tagManager.dataLayer pushValue:self.category
                                         forKey:@"category_name"];
}

// Override
- (void)viewWillAppear:(BOOL)animated {
  [super viewWillAppear:animated];
  [Utils pushOpenScreenEventWithScreenName:@"CategoryViewScreen"];
}

// Override
- (void)viewWillDisappear:(BOOL)animated {
  [super viewDidDisappear:animated];
  [Utils pushCloseScreenEventWithScreenName:@"CategoryViewScreen"];
}

// Override
- (BOOL)shouldAutorotateToInterfaceOrientation:
    (UIInterfaceOrientation)interfaceOrientation {
  if ([[UIDevice currentDevice] userInterfaceIdiom] ==
      UIUserInterfaceIdiomPhone) {
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
  } else {
    return YES;
  }
}

#pragma mark UITableViewDelegate methods

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  [tableView deselectRowAtIndexPath:indexPath animated:NO];
  if (indexPath.row < 0 || indexPath.row >= self.items.count) {
    NSLog(@"IndexPath %d out of bounds!", indexPath.row);
    return;
  }
  NSString *title = [NSString stringWithFormat:@"%@ %d", self.category, indexPath.row];
  UIImage *image = [self.items objectAtIndex:indexPath.row];
  ImageViewController *imageViewController =
      [[ImageViewController alloc] initWithNibName:nil
                                            bundle:nil
                                             title:title
                                             image:image];
  [imageViewController.view addSubview:imageViewController.imageView];
  [self.navController pushViewController:imageViewController animated:YES];
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

  static NSString *CellId = @"Cell";
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellId];
  if (!cell) {
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle
                                  reuseIdentifier:CellId];
  }

  cell.textLabel.text = [NSString stringWithFormat:@"%@ %d", self.category, indexPath.row];
  cell.textLabel.font = [UIFont systemFontOfSize:14];
  UIImage *image = [self.items objectAtIndex:indexPath.row];
  cell.detailTextLabel.text = [NSString stringWithFormat:@"%d x %d",
                               (int)image.size.width, (int)image.size.height];
  cell.detailTextLabel.font = [UIFont systemFontOfSize:12];
  return cell;
}

@end
