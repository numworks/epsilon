// #include <apps/home/apps_layout.h>
// // #include "../apps_container.h"
// #include "stddef.h"
// #include "string.h"
// #include "math.h"
// #include "app.h"
#include <apps/home/apps_layout.h>
#include "apps_container.h"
#include "global_preferences.h"
// #include "../exam_mode_configuration.h"

extern "C" {
#include <assert.h>
}

#ifdef NSPIRE_NEWLIB
const char * storage_name="/documents/nwstore.nws.tns";
#else
const char * storage_name="nwstore.nws";
#endif

const char * calc_storage_name="nwcalc.txt";
void * storage_address(); // ion/src/simulator/shared/platform_info.cpp
const char * retrieve_calc_history(); 
#if defined _FXCG  || defined NSPIRE_NEWLIB
bool save_calc_history();
void display_host_help();
int load_state(const char * fname);
#endif

// Additional code by B. Parisse for host file system support and persistence
// on Casio Graph 90/FXCG50 and TI Nspire
void erase_record(const char * name){
  unsigned char * ptr=(unsigned char *)storage_address();
  for (ptr+=4;;){ // skip header
    size_t L=ptr[1]*256+ptr[0];
    if (L==0) return;
    if (strcmp((const char *)ptr+2,name)==0){
      unsigned char * newptr=ptr;
      int S=0,erased=L;
      ptr+=L;
      for (;;){
	L=ptr[1]*256+ptr[0];
	if (L==0){
	  for (int i=0;i<erased;++i)
	    newptr[S+i]=0;
	  break;
	}
	// if (ptr>newptr+S)
	memmove(newptr+S,ptr,L);
	S+=L;
	ptr+=L;
      }
      return;
    }
    ptr+=L;
  }
}


// record filtering on read
void filter(unsigned char * ptr){
  unsigned char * newptr=ptr;
  int S; ptr+=4;
  for (S=4;;){
    size_t L=ptr[1]*256+ptr[0];
    if (L==0) break;
    int l=strlen((const char *)ptr+2);
    // filter py records
    if (l>3 && strncmp((const char *)ptr+2+l-3,".py",3)==0){
      // if (ptr>newptr+S)
      memmove(newptr+S,ptr,L);
      S+=L;
    }
#if 0 // def STRING_STORAGE
    if (l>5 && strncmp((const char *)ptr+2+l-5,".func",5)==0){
      int shift=l+4+13;
      Ion::Storage::Record * record=(Ion::Storage::Record *)malloc(1024);
      memcpy(record,ptr,L);
      //ExpressionModelHandle
      Poincare::Expression e=Poincare::Expression::Parse((const char *)ptr+shift,NULL);
      //ExpressionModel::setContent(Ion::Storage::Record * record, const char * c, Context * context, CodePoint symbol);
      Shared::ExpressionModel md;
      Ion::Storage::Record::ErrorStatus err=md.setExpressionContent(record, e);
      if (1){
	// if (ptr>newptr+S)
	int newL=record->value().size;
	memmove(newptr+S,record,newL);
	S+=newL;
      }
      free(record);
    }
#endif
    ptr+=L; 
  }
}


#ifdef NSPIRE_NEWLIB
#include "../../ion/src/simulator/fxcg/platform.h"
#include "../../ion/src/simulator/fxcg/menuHandler.h"
#include <ion/storage.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <string>

#include <ion.h>
#include <ion/events.h>
#include "../../ion/src/simulator/nspire/k_csdk.h"
#include "../calculation/calculation_store.h"

#define C_WHITE SDK_WHITE
#define C_BLACK SDK_BLACK
#define C_RED (31<<11)

int do_getkey(){
  os_wait_1ms(50);
  return getkey(0);
}

void dtext(int x,int y,int fg,const char * s){
  os_draw_string_medium(x,y,fg,SDK_WHITE,s);
}

void dclear(int c){
  os_fill_rect(0,0,LCD_WIDTH_PX,LCD_HEIGHT_PX,c);
}

void dupdate(){
  sync_screen();
}

const int storage_length=60000; // 60000 in Upsilon, 32768 in Epsilon
// k_storageSize = 60000; in ion/include/ion/internal_storage.h
extern void * last_calculation_history;

int load_state(const char * fname){
  FILE * f=fopen(fname,"rb");
  if (f){
    unsigned char * ptr=(unsigned char *)storage_address();
    fread(ptr,1,storage_length,f);
    fclose(f);
#ifdef FILTER_STORE
    filter(ptr);
#endif
    return 1;
  }
  return 0;
}

int save_state(const char * fname){
  save_calc_history();
  if (1 || Ion::Storage::sharedStorage()->numberOfRecords()){
    const unsigned char * ptr=(const unsigned char *)storage_address();
    // find store size
    int S=4; 
    for (ptr+=4;;){ // skip header
      size_t L=ptr[1]*256+ptr[0];  
      ptr+=L;
      S+=L;
      if (L==0) break;
    }
    S = ((S+1023)/1024)*1024;
#ifdef FILTER_STORE
    // keep only python scripts
    unsigned char * newptr=(unsigned char *) malloc(S);
    bzero(newptr,S);
    ptr=(const unsigned char *) storage_address();
    memcpy(newptr,ptr,4); ptr+=4;
    for (S=4;;){
      size_t L=ptr[1]*256+ptr[0];
      if (L==0) break;
      int l=strlen((const char *)ptr+2);
      if (l>3 && strncmp((const char *)ptr+2+l-3,".py",3)==0){
	memcpy(newptr+S,ptr,L);
	S+=L;
      }
      ptr+=L; 
    }
    S = ((S+1023)/1024)*1024;
    FILE * f;
    f=fopen(fname,"wb");
    if (f){
      fwrite(newptr,S,1,f);
      //fwrite(ptr+4,1,S-4,f);
      fclose(f);
      free(newptr);
      return S;
    }
    free(newptr);
    return 0;
#else
    ptr=(const unsigned char *)storage_address();
    FILE * f;
    f=fopen(fname,"wb");
    if (f){
      fwrite(ptr,S,1,f);
      //fwrite(ptr+4,1,S-4,f);
      fclose(f);
      return S;
    }
    return 0;
#endif
  }
  return 2;
}

#endif // NSPIRE_NEWLIB

#ifdef _FXCG
#include "../../ion/src/simulator/fxcg/platform.h"
#include "../../ion/src/simulator/fxcg/menuHandler.h"
#include "../calculation/calculation_store.h"
#include <ion/storage.h>

#include <gint/bfile.h>
#include <gint/display-cg.h>
#include <gint/gint.h>
#include <gint/display.h>
#include <gint/keyboard.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <string>

#include <ion.h>
#include <ion/events.h>
#define KEY_CTRL_OK KEY_EXE
int do_getkey(){
  return getkey().key;
}

const int storage_length=60000; // 60000 in Upsilon, 32768 in Epsilon
// k_storageSize = 60000; in ion/include/ion/internal_storage.h
extern void * last_calculation_history;

int load_state(const char * fname){
  FILE * f=fopen(fname,"rb");
  if (f){
    unsigned char * ptr=(unsigned char *)storage_address();
    ptr[3]=fgetc(f);
    ptr[2]=fgetc(f);
    ptr[1]=fgetc(f);
    ptr[0]=fgetc(f);
    fread(ptr+4,1,storage_length-4,f);
    fclose(f);
    return 1;
  }
  return 0;
}

int save_state(const char * fname){
  save_calc_history();
  if (Ion::Storage::sharedStorage()->numberOfRecords()){
#if 0
    unsigned short pFile[512];
    convert(fname,pFile);
    int hf = BFile_Open(pFile, BFile_WriteOnly); // Get handle
    // cout << hf << endl << "f:" << filename << endl; Console_Disp();
    if (hf<0){
      int l=storage_length;
      BFile_Create(pFile,0,&l);
      hf = BFile_Open(pFile, BFile_WriteOnly);
    }
    if (hf < 0)
      return 0;
    int l=BFile_Write(hf,storage_address(),storage_length);
    BFile_Close(hf);
    if (l==storage_length)
      return 1;
    return -1;
#else
    const unsigned char * ptr=(const unsigned char *)storage_address();
    // find store size
    int S=4; 
    for (ptr+=4;;){ // skip header
      size_t L=ptr[1]*256+ptr[0];  
      ptr+=L;
      S+=L;
      if (L==0) break;
    }
    S = ((S+1023)/1024)*1024;
    FILE * f=fopen(fname,"wb");
    if (f){
      ptr=(const unsigned char *) storage_address();
      fputc(ptr[3],f);
      fputc(ptr[2],f);
      fputc(ptr[1],f);
      fputc(ptr[0],f);
      //fwrite(ptr+4,1,S-4,f);
      fwrite(ptr+4,S-4,1,f);
      fclose(f);
      return S;
    }
    return 0;
#endif
  }
  return 2;
}
#endif // _FXCG

const char * retrieve_calc_history(){
#if defined _FXCG || defined NSPIRE_NEWLIB
  static bool firstrun=true;
  if (firstrun){
#ifdef _FXCG
    int l=gint_world_switch(GINT_CALL(load_state,storage_name));
#else
    int l=load_state(storage_name);
#endif
    if (l==0){
      display_host_help();
      // ((App*)m_app)->redraw();      
    }
    firstrun=false;
  }
#endif
  unsigned char * ptr=(unsigned char *)storage_address();
  for (ptr+=4;;){ // skip header
    size_t L=ptr[1]*256+ptr[0]; 
    if (L==0) return 0;
    if (strcmp((const char *)ptr+2,calc_storage_name)==0){
      const char * buf=(const char *)ptr+2+strlen(calc_storage_name)+1;
      return buf;
    }
    ptr += L;
  }
  return 0;
}

#if defined _FXCG  || defined NSPIRE_NEWLIB
bool save_calc_history(){
  if (!last_calculation_history)
    return false;
  erase_record(calc_storage_name);
  std::string s;
  Calculation::CalculationStore * store=(Calculation::CalculationStore *) last_calculation_history;
  int N=store->numberOfCalculations();
  for (int i=N-1;i>=0;--i){
    s += store->calculationAtIndex(i)->inputText();
    s += '\n';
  }
  if (s.empty())
    return false;
  Ion::Storage::Record::ErrorStatus  res= Ion::Storage::sharedStorage()->createRecordWithFullName(calc_storage_name,&s[0],s.size());
  if (res==Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable)
    return false;
  return true;
}

void confirm_load_state(const char * buf){
  dclear(C_WHITE);
  dtext(1,1, C_BLACK, "Loading from state file");
  dtext(1,17,C_BLACK,buf);
  dtext(1,33,C_BLACK,"Current context will be lost!");
  dtext(1,49,C_BLACK,"Press EXE to confirm");
  dupdate();
  int k=do_getkey();
  if (k==KEY_EXE || k==KEY_CTRL_OK){
#ifdef _FXCG
    int l=gint_world_switch(GINT_CALL(load_state,buf));
#else
    int l=load_state(buf);
#endif
    char buf2[]="0";
    buf2[0] += l;
    if (l==0)
      dtext(1,65,C_BLACK,"Error reading state");
    if (l==1)
      dtext(1,65,C_BLACK,"Success reading state");
    dtext(1,81,C_BLACK,buf2);
    dtext(1,97,C_BLACK,"Press any key");
    dupdate();
    do_getkey();
  }
}  

static void convert(const char * fname,unsigned short * pFile){
  for ( ;*fname;++fname,++pFile)
    *pFile=*fname;
  *pFile=0;
}

struct file {
  std::string s;
  int length;
  bool isdir;
};

void host_scripts(std::vector<file> & v,const char * dirname,const char * extension){
  v.clear();
  file f={".._parent_dir",0,true};
  if (strlen(dirname)>1)
    v.push_back(f);
  DIR *dp;
  struct dirent *ep;  
  dp = opendir (dirname);
  int l=extension?strlen(extension):0;
  if (dp != NULL){
    int t;
    while ( (ep = readdir (dp)) ){
      if (strlen(ep->d_name)>=1 && ep->d_name[0]=='.')
	continue;
      f.s=ep->d_name;
      if (f.s=="@MainMem")
	continue;
#ifdef NSPIRE_NEWLIB
      DIR * chk=opendir((dirname+f.s).c_str());
      f.isdir=true;
      if (chk)
	closedir(chk);
      else
	f.isdir=false; 
#else
      f.isdir=ep->d_type==DT_DIR;
#endif
#if 1
      if (f.isdir)
	f.length=0;
      else {
	struct stat st;
	stat((dirname+f.s).c_str(), &st);
	f.length = st.st_size;
	if (f.length>=32768)
	  continue;
      }
#else
      f.length=f.isdir?0:-1;
#endif
      if (f.isdir || !extension)
	v.push_back(f);
      else {
	t=strlen(ep->d_name);
	if (t>l && strncmp(ep->d_name+t-l,extension,l)==0)
	  v.push_back(f);
      }
    }
    closedir (dp);
  }
}

void nw_scripts(std::vector<file> & v,const char * extension){
  v.clear();
#if 0
 int n=Ion::Storage::sharedStorage()->numberOfRecords();
 for (int i=0;i<n;++i){
   v.push_back(Ion::Storage::sharedStorage()->recordAtIndex(i).fullName());
 }
#else
  const unsigned char * ptr=(const unsigned char *)storage_address();
  int l=extension?strlen(extension):0;
  for (ptr+=4;;){ // skip header
    size_t L=ptr[1]*256+ptr[0]; 
    ptr+=2; 
    if (L==0) break;
    L-=2;
    file f={(const char *)ptr,(int)L,false};
    if (!extension)
      v.push_back(f);
    else {
      int namesize=strlen((const char *)ptr);
      if (namesize>l && strncmp((const char *)ptr+namesize-l,extension,l)==0)
	v.push_back(f);
    }
    ptr+=L;
  }
#endif
}

int copy_nw_to_host(const char * nwname,const char * hostname){
#ifdef NSPIRE_NEWLIB
  int s=strlen(hostname);
  if (s<4 || strncmp(hostname+s-4,".tns",4)){
    std::string S(hostname);
    S+=".tns";
    return copy_nw_to_host(nwname,S.c_str());
  }
#endif
  const unsigned char * ptr=(const unsigned char *)storage_address();
  for (ptr+=4;;){ // skip header
    size_t L=ptr[1]*256+ptr[0]; 
    if (L==0) return 3; // not found
    //dclear(C_WHITE);
    //dtext(1,1,C_BLACK,ptr+2);
    //dtext(1,17,C_BLACK,nwname);
    //dupdate();
    //getkey();
    if (strcmp((const char *)ptr+2,nwname)){
      ptr += L;
      continue;
    }
    ptr+=2; 
    L-=2;
    int l=strlen((const char *)ptr);
    ptr += l+2;
    L -= l;
    L = 2*((L+1)/2);
    FILE * f=fopen(hostname,"wb");
    if (!f)
      return 2;
    fwrite(ptr,1,L,f);
    fclose(f);
    return 0;
  }
  return 1;
}

int copy_host_to_nw(const char * hostname,const char * nwname,int autoexec){
  FILE * f=fopen(hostname,"rb");
  if (!f)
    return -1;
  std::vector<unsigned char> v(1,autoexec?1:0);
  for (;;){
    unsigned char c=fgetc(f);
    if (feof(f)){
      if (c>=' ' && c<=0x7e)
	v.push_back(c);
      break;
    }
    if (c==0xa && !v.empty() && v.back()==0xd)
      v.back()=0xa;
    else
      v.push_back(c);
  }
  if (!v.empty() && v.back()!=0xa)
    v.push_back(0xa);
  v.push_back(0);
  fclose(f);
  if (Ion::Storage::sharedStorage()->hasRecord(Ion::Storage::sharedStorage()->recordNamed(nwname)))
    Ion::Storage::sharedStorage()-> destroyRecord(Ion::Storage::sharedStorage()->recordNamed(nwname));
  Ion::Storage::Record::ErrorStatus  res= Ion::Storage::sharedStorage()->createRecordWithFullName(nwname,&v.front(),v.size());
  if (res==Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable)
    return -2;
  return 0;
}

bool filesort(const file & a,const file & b){
  if (a.isdir!=b.isdir)
    return a.isdir;
  return a.s<b.s;
}

void display_host_help();
void display_host_help(){
  dclear(C_WHITE);
  dtext(1,2, C_BLACK,"You can save up to 9 Upsilon calculator states:");
  dtext(1,38,C_BLACK,"You can manage and exchange Upsilon records");
  dtext(1,56,C_BLACK,"or restore states: from home, type VARS");
#ifdef _FXCG
  dtext(1,20,C_BLACK,"From home, type STO (above AC/ON) then 1 to 9");
  dtext(1,74,C_BLACK,"You can reset Upsilon state from Casio MENU");
  dtext(1,92,C_BLACK,"Select Memory, type F2, then erase nwstore.nws");
  dtext(1,110,C_RED, "From home, type MENU or DEL to leave Upsilon");
#else
  dtext(1,20,C_BLACK,"From home, type STO (ctrl VAR) then 1 to 9");
  dtext(1,74,C_BLACK,"You can reset Upsilon state from Home 2");
  dtext(1,92,C_BLACK,"Erase nwstore.nws in My documents");
  dtext(1,110,C_RED ,"Type Ctrl K to shutdown screen, ON to restore");
  dtext(1,128,C_RED ,"From home, type ON or doc to leave Upsilon");
  os_wait_1ms(100);
#endif
  dtext(1,150,C_BLACK,"Upsilon: a fork of Epsilon (c) Numworks");
  dtext(1,168,C_BLACK,"https://getupsilon.web.app/");
  dtext(1,186,C_BLACK,"creativecommons.org license CC BY-NC-SA 4.0");
  dtext(1,204,C_BLACK,"Persistance and host filesystem (c) B. Parisse");
  dupdate();
  do_getkey();
}

extern "C" void host_filemanager();
void host_filemanager(){
  int posnw=0,startnw=0,poshost=0,starthost=0;
  bool nw=true,reload=true;
  std::vector<file> v,w;
#ifdef NSPIRE_NEWLIB
  std::string hostdir="/documents/";
#else
  std::string hostdir="/";
#endif
  bool onlypy=true;
  for (;;){
    if (reload){
      nw_scripts(v,onlypy?".py":0); 
      sort(v.begin(),v.end(),filesort);
#ifdef NSPIRE_NEWLIB
      host_scripts(w,hostdir.c_str(),onlypy?".py.tns":0);
#else
      host_scripts(w,hostdir.c_str(),onlypy?".py":0);
#endif
      sort(w.begin(),w.end(),filesort);
      reload=false;
    }
    dclear(C_WHITE);
    dtext(1,1, C_BLACK,"EXIT: leave; key 1 to 9: load state from file");
#ifdef _FXCG
    dtext(1,17,C_BLACK,"Cursor keys: move, /: rootdir, OPTN: all/py files");
#else
    dtext(1,17,C_BLACK,"Cursor keys: move, /: rootdir");
#endif
    dtext(1,33,C_BLACK,"EXE or STO key: copy selection from/to host");
    dtext(1,49,C_BLACK,("Upsilon records             Host "+hostdir).c_str());
    int nitems=9;
    if (posnw<0)
      posnw=v.size()-1;
    if (posnw>=int(v.size()))
      posnw=0;
    if (posnw<startnw || posnw>startnw+nitems)
      startnw=posnw-4;
    if (startnw>=int(v.size())-nitems)
      startnw=v.size()-nitems;
    if (startnw<0)
      startnw=0;
    if (v.empty())
      nw=false;
    for (int i=0;i<=nitems;++i){
      int I=i+startnw;
      if (I>=int(v.size()))
	break;
      dtext(1,65+16*i,(nw && I==posnw)?C_RED:C_BLACK,v[I].s.c_str());
      char buf[256];
      sprintf(buf,"%i",v[I].length);
      dtext(90,65+16*i,(nw && I==posnw)?C_RED:C_BLACK,buf);
    }
    if (w.empty())
      nw=true;
    if (poshost<0)
      poshost=w.size()-1;
    if (poshost>=int(w.size()))
      poshost=0;
    if (poshost<starthost || poshost>starthost+nitems)
      starthost=poshost-4;
    if (starthost>=int(w.size())-nitems)
      starthost=w.size()-nitems;
    if (starthost<0)
      starthost=0;
    for (int i=0;i<=nitems;++i){
      int I=i+starthost;
      if (I>=int(w.size()))
	break;
      std::string fname=w[I].s;
      if (fname.size()>16)
	fname=fname.substr(0,16)+"...";
      dtext(192,65+16*i,(!nw && I==poshost)?C_RED:C_BLACK,fname.c_str());
      if (w[I].isdir)
	dtext(154,65+16*i,(!nw && I==poshost)?C_RED:C_BLACK,"<dir>");
      else {
	char buf[256];
	sprintf(buf,"%i",w[I].length);
#ifdef _FXCG
	dtext(340,65+16*i,(!nw && I==poshost)?C_RED:C_BLACK,buf);
#else
	dtext(285,65+16*i,(!nw && I==poshost)?C_RED:C_BLACK,buf);
#endif
      }
    }    
    dupdate();
    int key=do_getkey();
    if (key==KEY_EXIT || key==KEY_MENU)
      break;
    if (key==KEY_OPTN || key=='\t'){
      onlypy=!onlypy;
      reload=true;
      continue;
    }
    if (key==KEY_DIV){
#ifdef NSPIRE_NEWLIB
      hostdir="/documents/";
#else
      hostdir="/";
#endif
      reload=true;
      continue;
    }
    if (key==KEY_DEL){
      if (!nw && w[poshost].isdir) // can not remove directory
	continue;
      dclear(C_WHITE);
      dtext(1,17,C_BLACK,nw?"About to suppress Upsilon record:":"About to suppress Host file:");
      dtext(1,33,C_BLACK,(nw?v[posnw].s:w[poshost].s).c_str());
      dtext(1,49,C_BLACK,"Press EXE or OK to confirm");
      dupdate();
      int ev=do_getkey();
      if (ev!=KEY_EXE && ev!=KEY_CTRL_OK)
	continue;
      if (nw){
#if 1
	erase_record(v[posnw].s.c_str());
#else
	char buf[256];
	strcpy(buf,v[posnw].s.c_str());
	int l=strlen(buf)-4;
	buf[l]=0;
	Ion::Storage::sharedStorage()-> destroyRecordWithBaseNameAndExtension(buf,buf+l+1);
#endif
      }
      else
	remove((hostdir+w[poshost].s).c_str());
      reload=true;
    }
    if (key==KEY_LEFT){
      nw=true;
      continue;
    }
    if (key==KEY_RIGHT){
      nw=false;
      continue;
    }
    if (key==KEY_PLUS){
      if (nw)
	posnw+=5;
      else 
	poshost+=5;
      continue;
    }
    if (key==KEY_MINUS){
      if (nw)
	posnw-=5;
      else 
	poshost-=5;
      continue;
    }
    if (key==KEY_DOWN){
      if (nw)
	++posnw;
      else
	++poshost;
      continue;
    }
    if (key==KEY_UP){
      if (nw)
	--posnw;
      else 
	--poshost;
      continue;
    }
    int autoexec = key==KEY_EXE || key==KEY_CTRL_OK;
    if (key==KEY_STORE || autoexec){
      if (nw && posnw>=0 && posnw<int(v.size())){
	size_t i=0;
	for (;i<w.size();++i){
	  if (v[posnw].s==w[i].s)
	    break;
	}
	if (i<w.size()){
	  dclear(C_WHITE);
	  if (w[i].isdir){
	    dtext(1,33,C_BLACK,"Can not overwrite a Host directory!");
	    dupdate();
	    do_getkey();
	    continue;
	  }
	  dtext(1,33,C_BLACK,"Script exists in Host directory!");
	  dtext(1,49,C_BLACK,"Press EXE to confirm overwrite");
	  dupdate();
	  int k=do_getkey();
	  if (k!=KEY_EXE && k!=KEY_CTRL_OK)
	    continue;
	}
#ifdef _FXCG
	int l=gint_world_switch(GINT_CALL(copy_nw_to_host,v[posnw].s.c_str(),(hostdir+v[posnw].s).c_str()));
#else
	int l=copy_nw_to_host(v[posnw].s.c_str(),(hostdir+v[posnw].s).c_str());
#endif
	char buf2[]="0";
	buf2[0] = '0'+l;
	dclear(C_WHITE);
	dtext(1,65,C_BLACK,(hostdir+v[posnw].s).c_str());
	dtext(1,81,C_BLACK,buf2);
	dupdate();
	if (l!=0)
	  do_getkey();
	reload=true;
      }
      else if (!nw && poshost>=0 && poshost<int(w.size())){
	if (w[poshost].s==".._parent_dir"){
	  // one level up
	  for (int j=hostdir.size()-2;j>=0;--j){
	    if (hostdir[j]=='/'){
	      hostdir=hostdir.substr(0,j+1);
	      break;
	    }
	  }
	  reload=true;
	  continue;
	}
	// lookup if poshost is in directories
	if (w[poshost].isdir){
	  hostdir += w[poshost].s;
	  hostdir += "/";
	  reload=true;
	  continue;
	}
	size_t i;
	std::string fname=w[poshost].s;
#ifdef NSPIRE_NEWLIB
	if (fname.size()>4 && fname.substr(fname.size()-4,4)==".tns")
	  fname=fname.substr(0,fname.size()-4);
#endif
	for (i=0;i<v.size();++i){
	  if (fname==v[i].s)
	    break;
	}
	if (i<v.size()){
	  dclear(C_WHITE);
	  dtext(1,33,C_BLACK,"Script exists in Upsilon!");
	  dtext(1,49,C_BLACK,"Press EXE to confirm overwrite");
	  dupdate();
	  int k=do_getkey();
	  if (k!=KEY_EXE && k!=KEY_CTRL_OK)
	    continue;
	}
	std::string nwname=fname;
	if (fname.size()>12){
	  dclear(C_WHITE);
	  dtext(1,33,C_BLACK,"Host filename too long");
	  dtext(1,49,C_BLACK,fname.c_str());
	  dupdate();
	  do_getkey();
	  continue;
	}
	if (fname.size()>4 && fname.substr(fname.size()-4,4)==".nws")
	  confirm_load_state((hostdir+fname).c_str());
	else {
#ifdef _FXCG
	  gint_world_switch(GINT_CALL(copy_host_to_nw,(hostdir+fname).c_str(),nwname.c_str(),autoexec));
#else
	  copy_host_to_nw((hostdir+w[poshost].s).c_str(),nwname.c_str(),autoexec);
#endif
	}
	reload=true;
      }
    }
    if (key>=KEY_1 && key<=KEY_9){
#ifdef NSPIRE_NEWLIB
      char buf[]="nwstate0.nws.tns";
#else
      char buf[]="nwstate0.nws";
#endif
      buf[7]='1'+(key-KEY_1);
      confirm_load_state(buf);
      reload=true;
    }
  }
}
#endif // FXCG || NSPIRE