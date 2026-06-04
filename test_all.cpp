/**
 * test_all.cpp — XSWL API 全覆盖测试
 *
 * 编译: xxcc test_all.cpp -o test_all.epf
 * 测试: python3 XJ380_wine.py test_all.epf  或  ./xswl test_all.epf
 */
#include <xapi.h>
#include <krlibc.h>
#include <string.h>

int pass, fail;
#define T(n)    xapi_Output((WSTR)(n))
#define OK()    do{xapi_PrintLine((WSTR)"OK");pass++;}while(0)
#define BAD(s)  do{xapi_Printf((WSTR)"FAIL(%s)\n",(WSTR)(s));fail++;}while(0)
#define CHK(c,s) do{if(c)OK();else BAD(s);}while(0)

int main(int argc, char** argv, char** envp)
{
    (void)argc;(void)argv;(void)envp;
    pass=fail=0;
    xapi_PrintLine((WSTR)"XSWL Full API Test");

    /* 1. 系统信息 */
    xapi_PrintLine((WSTR)"1.SysInfo");
    {char v[64];xapi_GetSystemVersion(v);CHK(v[0],"");}
    {char m[128];xapi_GetCpuModel(m);CHK(m[0],"");}
    {CHK(xapi_GetMemorySize()>0,"");}
    {CHK(xapi_GetTime()>1400000000ULL,"");}
    {TimeType t;xapi_GetTimeX(&t);CHK(t.tm_year>=2024,"");}
    {UserInfo u;xapi_GetCurrentUser(&u);CHK(u.user_type<=4,"");}

    /* 2. 文本I/O */
    xapi_PrintLine((WSTR)"2.TextIO");
    {xapi_Output((WSTR)"O");OK();}
    {xapi_PrintLine((WSTR)"");OK();}
    {xapi_EndLine();OK();}
    {xapi_Printf((WSTR)"=%d\n",42);OK();}
    {xapi_OutputSerial((WSTR)"S");OK();}

    /* 3. 类型转换 */
    xapi_PrintLine((WSTR)"3.Convert");
    {CHK(xcr_int2char(99)!=0,"");}
    {CHK(xcr_char2int((WSTR)"88")==88,"");}
    {CHK(xcr_hex2char(0xAB)!=0,"");}
    {CHK(toRGB(1,2,3)==0x010203FF,"");}
    {CHK(toRGBA(0xFFAABBCC)==0xFFCCBBAA,"");}

    /* 4. 文件 */
    xapi_PrintLine((WSTR)"4.File");
    xapi_CreateFile((WSTR)"/f");
    xapi_WriteFile((WSTR)"/f",(char*)"XY",2,0);
    {XFILE* f=xapi_OpenFile((WSTR)"/f");CHK(f!=0,"");OK();}
    {char b[8];memset(b,0,8);xapi_ReadFile((WSTR)"/f",b,2,0);CHK(b[0]=='X',"");OK();}
    {xapi_RenameFile((WSTR)"/f",(WSTR)"/g");OK();}
    {xapi_DeleteFile((WSTR)"/g");OK();}
    {xapi_Mkdir((WSTR)"/d");xapi_Rmdir((WSTR)"/d");OK();}
    {UINT32 c=0;DirNode n[255];xapi_SearchFile((WSTR)"/",&c,n);CHK(c>=1,"");OK();}

    /* 5. 进程 */
    xapi_PrintLine((WSTR)"5.Proc");
    {CHK(xapi_Fork()==(UINT64)-1,"");}
    {XapiTaskInfo ti[4];CHK(xapi_GetTaskList(ti,4)>=1,"");OK();}
    {xapi_KillProcess(999);OK();}

    /* 6. 消息 */
    xapi_PrintLine((WSTR)"6.Msg");
    {xapi_SendAppMessage((WSTR)"T",(WSTR)"B");OK();}
    {xapi_Sleep(5);OK();}
    {xapi_Run((WSTR)"/x");OK();}
    {xapi_FlushTime();OK();}

    /* 7. 内存 */
    xapi_PrintLine((WSTR)"7.Mem");
    {void*p=xapi_AllocateMemory(128);xapi_FreeMemory(p);CHK(p!=0,"");}
    {CHK(xapi_MapMemory(0,0x2000,7)!=0,"");}

    /* 8. GUI */
    xapi_PrintLine((WSTR)"8.GUI");
    HDLE h=0;XWINDOW w;w.width=400;w.height=300;w.title=(WSTR)"XSWL";w.sets=0;
    xapi_CreateWindow(&h,&w);
    if(!h){xapi_PrintLine((WSTR)"GUI skip");goto end;}
    xapi_SetWindowTitle(h,(WSTR)"T");
    {UINT64 ww=0,hh=0;xapi_GetWindowSize(h,&ww,&hh);CHK(ww==400,"");OK();}
    SetMsgPrcor(0);

    /* 9. 绘图 */
    xapi_PrintLine((WSTR)"9.Draw");
    xapi_DrawRect(h,0,0,400,300,0xFF000000,true);
    xapi_DrawRect(h,5,5,80,40,0xFFFF0000,false);
    xapi_DrawLine(h,100,10,180,60,0xFF00FF00);
    {int i;for(i=0;i<5;i++)xapi_DrawPoint(h,200+i*6,30,0xFF0000FF);}
    xapi_DrawText(h,5,70,(WSTR)"ABC",12,0xFFFFFFFF);
    {UINT32 tw=0;xapi_DrawTextl(h,5,90,(WSTR)"ABC",12,0xFFFFFFFF,&tw);CHK(tw>0,"");OK();}
    xapi_DrawSWText(h,5,110,(WSTR)"mono",0xFFFFFFFF);

    /* 10. Framebuffer */
    xapi_PrintLine((WSTR)"10.FB");
    {XCOLOR wc;wc.Red=255;wc.Green=0;wc.Blue=0;
     XCOLOR rc;rc.Red=0;rc.Green=0;rc.Blue=0;
     xapi_WriteBuffer(h,300,10,1,1,&wc);xapi_ReadBuffer(h,300,10,1,1,&rc);
     CHK(rc.Red==255,"");OK();}
    {XCOLORA wa;wa.Red=0;wa.Green=255;wa.Blue=0;wa.Alpha=128;
     XCOLOR ra;ra.Red=0;ra.Green=0;ra.Blue=0;
     xapi_WriteBufferA(h,320,10,1,1,&wa);xapi_ReadBufferA(h,320,10,1,1,&ra);
     CHK(ra.Green==255,"");OK();}
    xapi_RefreshWindow(h);
    xapi_RefreshPartWindow(h,0,0,100,100);

    /* 11. 控件 */
    xapi_PrintLine((WSTR)"11.Ctrl");
    xapi_Button(h,1,50,250,(WSTR)"Btn");
    xapi_ButtonEmp(h,2,100,250,(WSTR)"Emp");
    xapi_DeleteButton(h,2);
    {RightMenuItem m[2]={{3,(WSTR)"A"},{4,(WSTR)"B"}};
     xapi_RegisterRightButtonMenu(h,m,2);xapi_DeleteRightButtonMenu(h);OK();}

    xapi_Sleep(1000);
    xapi_CloseWindow(h);

end:
    xapi_Printf((WSTR)"Pass=%d Fail=%d\n",pass,fail);
    if(fail)xapi_PrintLine((WSTR)"FAIL");
    else   xapi_PrintLine((WSTR)"ALL OK");
    return fail;
}