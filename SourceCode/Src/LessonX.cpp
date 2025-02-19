/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#include <Stdio.h>
#include "CommonClass.h"
#include "LessonX.h"
#include "CTankPlayer.h"
#include "CBullet.h"
#include "CWeapon.h"
#include <iostream>
#include <vld.h>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
//
CGameMain		g_GameMain;

//==============================================================================
//
// 大体的程序流程为：GameMainLoop函数为主循环函数，在引擎每帧刷新屏幕图像之后，都会被调用一次。

//==============================================================================
//
// 构造函数
CGameMain::CGameMain()
{
	m_iGameState			=	0;
	m_pBattleCity           =   new CSprite("battleCityText");
	m_pSpaceStart           =   new CSprite("spaceStart");

    m_pScore                =   new CTextSprite("score");
	m_pHight                =   new CTextSprite("hight");
    m_pEnemy                =   new CTextSprite("enemyNum");
    m_iScore                =   0;//分数
    m_iEnemy                =   0;//敌人数量
    m_iHight                =   0;
    m_fDeltaTime            =   0.f;
    m_bWallStart            =   true;
    m_bPlayerSpeed          =   true;
    m_bEnemySpeed           =   true;
    m_bTracePlayer          =   true;
    m_bFish                 =   false;
    m_bInvincibility        =   true;
}
//==============================================================================
//
// 析构函数
CGameMain::~CGameMain()
{
    _CrtSetBreakAlloc(106);

    free(m_pBattleCity);
    free(m_pSpaceStart);
    free(m_pScore);
    free(m_pHight);
    free(m_pEnemy);
}

//==============================================================================
//
// 游戏主循环，此函数将被不停的调用，引擎每刷新一次屏幕，此函数即被调用一次
// 用以处理游戏的开始、进行中、结束等各种状态.
// 函数参数fDeltaTime : 上次调用本函数到此次调用本函数的时间间隔，单位：秒
void CGameMain::GameMainLoop( float	fDeltaTime )
{
	switch( GetGameState() )
	{
		// 初始化游戏，清空上一局相关数据
	case 1:
		{
			GameInit();
			SetGameState(2); // 初始化之后，将游戏状态设置为进行中
		}
		break;

		// 游戏进行中，处理各种游戏逻辑
	case 2:
		{
			// TODO 修改此处游戏循环条件，完成正确游戏逻辑
			if(!m_pTankPlayer->IsDead() && !m_pAim_nor->IsDead() && m_fDeltaTime<30)
			{
				GameRun( fDeltaTime );
			}
			else // 游戏结束。调用游戏结算函数，并把游戏状态修改为结束状态
			{
				SetGameState(0);
				GameEnd();
			}
		}
		break;

		// 游戏结束/等待按空格键开始
	case 0:
	default:
		break;
	};
}
//=============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
void CGameMain::GameInit()
{
    m_pBattleCity->SetSpriteVisible(false);
    m_pSpaceStart->SetSpriteVisible(false);

    m_pTankPlayer = new CTankPlayer("myplayer");//新建一个名字是myPlayer的我方坦克对象
	m_pTankPlayer->CloneSprite("player");       //我方坦克克隆在funcode模板中存在的名字为player的坦克，表示新建的坦克对象有现在精灵的所有属性
	m_pTankPlayer->Init();

//	m_pTankEnemy = new CTankEnemy("enemy");
//  m_pTankEnemy->Init();

    LoadMap();

    m_vWeapon.push_back(m_pTankPlayer);

    m_pAim_nor = new CWeapon("myaim_nor");
	m_pAim_nor->CloneSprite("aim_nor");
	m_vWeapon.push_back(m_pAim_nor);
	m_pAim_nor->SetSpriteCollisionReceive(true);
	m_pAim_nor->SetSpritePosition(0.f,20.f);

    m_iBulletNum    =   0;
	m_iTankEnemyNumber  =  0;
	m_fTankEnemyTime = 4.f;
	m_iScore = 0;
	m_iHight = 0;
	m_iEnemy = 0;
    m_fDeltaTime = 0.f;

    FILE * fp =fopen("save.dat","r+");
    if(fp)
    {
        fread(&m_iHight,sizeof(int),1,fp);
        fclose(fp);
    }
    //m_pHight = new CTextSprite("hight");");
    m_pHight->SetTextValue(m_iHight);

}
//=============================================================================
//
// 每局游戏进行中
void CGameMain::GameRun( float fDeltaTime )
{
//    if(m_pTankEnemy)
//	{
//		m_pTankEnemy->OnMove(fDeltaTime);
//		m_pTankEnemy->OnFire(fDeltaTime);
//	}
    AddTankEnemy(fDeltaTime);

    for(int i=0;i<m_vWeapon.size();i++)
	{
		if(!m_vWeapon[i]->IsDead())
        {
            m_vWeapon[i]->OnMove(fDeltaTime);
  			m_vWeapon[i]->OnFire(fDeltaTime);
        }
		else
		{
			DeleteWeaponByName(m_vWeapon[i]->GetName());
		}
	}
	m_fDeltaTime += fDeltaTime;
    m_pScore->SetTextValue(m_iScore);
	m_pHight->SetTextValue(m_iHight);
	m_pEnemy->SetTextValue(m_iEnemy);

	TracePlayer(fDeltaTime);
}
//=============================================================================
//
// 本局游戏结束
void CGameMain::GameEnd()
{
    DeleteAllSprite();
    FILE * fp =fopen("save.dat","w+");
    if(m_iScore>m_iHight)
        fwrite(&m_iScore,sizeof(int),1,fp);
    fclose(fp);

    m_pBattleCity->SetSpriteVisible(true);
	m_pSpaceStart->SetSpriteVisible(true);
	SetGameState(0);;
}
//=============================================================================
//
void CGameMain::OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress )
{
    //按空格键开始游戏
    if( GetGameState() == 0 ){
        if( iKey == KEY_SPACE ){
            m_iGameState = 1;
        }
    }
    //操控myplayer
    if(m_iGameState == 2)
	{
		m_pTankPlayer->OnMove(iKey, true);
	}
	switch(iKey)
	{
    case KEY_J://判断按下键是够为J键
		m_pTankPlayer->OnFire();
        break;
    case KEY_0:
        PlayerSpeedFast();
        m_bPlayerSpeed=!m_bPlayerSpeed;
        break;
    case KEY_1:
        PlayerTP();
        break;
    case KEY_2:
        if(m_bInvincibility)
        {
            m_pTankPlayer->MakeHp(-1);
        }
        else
        {
            m_pTankPlayer->MakeHp(2);
        }
        m_bInvincibility=!m_bInvincibility;
        break;
    case KEY_3:
        m_bFish=!m_bFish;
        break;
    case KEY_4:
        RigidityMap();
        m_bWallStart=!m_bWallStart;
        break;
    case KEY_6:
        StopEnemy();
        m_bEnemySpeed=!m_bEnemySpeed;
        break;
    case KEY_9:
        m_bTracePlayer=!m_bTracePlayer;
        break;
	}
//    if(iKey == KEY_J)
//	{
//		m_pTankPlayer->OnFire();
//	}

}

void CGameMain::OnKeyUp(const int iKey)
{
    //操控myplayer
	if(m_iGameState == 2)
	{
		m_pTankPlayer->OnMove(iKey, false);
    }
}

void CGameMain::OnSpriteColWorldLimit(const char* szName, const int iColSide)
{
//	if(strstr(szName,"player") != NULL) //判断碰到世界边界的坦克是否为我方坦克
//	{
//        m_pTankPlayer->SetSpriteLinearVelocity(0,0);
//	}
////	if(m_pTankEnemy&&strcmp(m_pTankEnemy->GetName(),szName)==0)
////	{
////		m_pTankEnemy->OnMove();
////	}
//    else if(strstr(szName,"enemy") != NULL)
//	{
//		CWeapon* pEnemy = FindWeaponByName(szName);
//		pEnemy->SetSpriteLinearVelocity(0.f,0.f);
//		pEnemy->OnMove();
//	}else
	if(strstr(szName,"bullet") != NULL)
	{
		CWeapon* pBullet = FindWeaponByName(szName);
		pBullet->SetHp(-2);
	}
}

void CGameMain::AddBullet( int iDir,float fPosX,float fPosY ,int iOwner)
{
	char* szName = CSystem::MakeSpriteName("bullet",m_iBulletNum);//创建坦克名字
	CBullet* pBullet = new CBullet(szName);
	if(m_bFish&&iOwner == 1)
    {
        pBullet->CloneSprite("fish");
	}
	else
    {
        pBullet->CloneSprite("bullet");
    }
	pBullet->SetSpriteWorldLimit(WORLD_LIMIT_NULL,-26, -22, 26, 22); //设置世界边界
	pBullet->SetSpritePosition(fPosX,fPosY);
	pBullet->SetSpriteCollisionSend(true); //设置接收碰撞
	pBullet->OnMove(iDir);
	m_iBulletNum++; //子弹个数加1
	if(iOwner == 1)
	{
		pBullet->SetOwner(1);//1表示我方坦克发射的子弹
	}
	else
	{
		pBullet->SetOwner(0); //0表示地方坦克发射的子弹
	}
    m_vWeapon.push_back(pBullet);
}
void CGameMain::LoadMap()
{
	char* szName;
	int i,j;
	float x,y;
	for(i=0;i<11;i++)
	{
		for(j=0;j<13;j++)
		{
			if(g_iMap[i][j]!=0)
			{
				szName = CSystem::MakeSpriteName("wall",j+i*13+i);//重新起名
				CWeapon* pWall = new CWeapon(szName);//新建对象
				pWall->CloneSprite("wall"); //克隆墙块
				pWall->SetSpriteCollisionActive(0,1); //设置为接受碰撞
				pWall->SetSpriteCollisionResponse(COL_RESPONSE_CUSTOM);
				x =float(-24+4*j);
				y =float(-20+4*i);
				pWall->SetSpritePosition(x,y);
                m_vWeapon.push_back(pWall);
			}
		}
	}
}

void CGameMain::RigidityMap()
{
    for(auto wall : m_vWeapon)
    {
        const char* szName = wall->GetName();
        if(strstr(szName,"wall") != NULL)
        {
            if(m_bWallStart)
            {
                wall->SetSpriteCollisionActive(0,0);
            }
            else
            {
                wall->SetSpriteCollisionActive(0,1);
            }
        }
    }
}

void CGameMain::PlayerSpeedFast()
{
    for(auto player : m_vWeapon)
    {
        const char* szName = player->GetName();
        if(strstr(szName,"player") != NULL)
        {
            if(m_bPlayerSpeed)
            {
                player->SetSpeed(12);
            }
            else
            {
                player->SetSpeed(8);
            }
        }
    }
}

void CGameMain::StopEnemy()
{
    for(auto enemy : m_vWeapon)
    {
        const char* szName = enemy->GetName();
        if(strstr(szName,"enemy") != NULL)
        {
            if(m_bEnemySpeed)
            {
                enemy->SetSpeed(0);
            }
            else
            {
                enemy->SetSpeed(8);
            }
        }
    }
}

void CGameMain::PlayerTP()
{
    int x = CSystem::RandomRange(-26,26);
    int y = CSystem::RandomRange(-22,22);
    for(auto player : m_vWeapon)
    {
        const char* szName = player->GetName();
        if(strstr(szName,"player") != NULL)
        {
            player->SetSpritePosition(x,y);
        }
    }
}

void CGameMain::TracePlayer(float fDeltaTime)
{
    for(auto vWeapon : m_vWeapon )
    {
        // 追踪
        if(m_bTracePlayer==false)
        {
            vWeapon->TrackMove();
            vWeapon->OnFire(fDeltaTime);
        }
        else
        {
            vWeapon->OnMove(fDeltaTime);
            vWeapon->OnFire(fDeltaTime);
        }
    }
}

CWeapon* CGameMain::FindWeaponByName(const char* szName)//根据名字查找到对象
{
	for(int i=0; i<m_vWeapon.size(); i++)
	{
        if(strcmp(szName,m_vWeapon[i]->GetName()) == 0)
        {
            return m_vWeapon[i];
        }
	}
}

void CGameMain::DeleteWeaponByName(const char* szName)//根据名字把精灵从容器中删除
{
	for(vector<CWeapon*>::iterator it=m_vWeapon.begin();it!=m_vWeapon.end();)
	{
		CWeapon* cw =*it;
		if(strcmp(szName,cw->GetName()) == 0)
		{
			m_vWeapon.erase(it);
			cw->DeleteSprite();
			delete cw;
			break;
		}
		else
		{
			it++;
		}
	}
}

void CGameMain::AddTankEnemy(float fDeltaTime)
{
	m_fTankEnemyTime += fDeltaTime;
	if(m_fTankEnemyTime > 5)
	{
		char* szName = CSystem::MakeSpriteName("enemy",m_iTankEnemyNumber);
		CTankEnemy* m_pTankEnemy = new CTankEnemy(szName);
		m_pTankEnemy->CloneSprite("enemy");
		m_pTankEnemy->Init();
		m_pTankEnemy->SetSpriteVisible(true);
        m_iTankEnemyNumber++;
		m_vWeapon.push_back(m_pTankEnemy);  //把创建的敌方坦克插入到容器中
		m_fTankEnemyTime=0.f;
	    m_iEnemy++;//坦克数量增长
	}
}

void CGameMain::OnSpriteColSprite(const char *szSrcName, const char *szTarName)
{
	CWeapon* tarSprite = FindWeaponByName(szTarName);
	if(strstr(szSrcName,"bullet") != NULL)//发送碰撞为子弹
	{
		CBullet *tmpBullet = (CBullet*)FindWeaponByName(szSrcName);
		tmpBullet->OnSpriteColSprite(tarSprite);
		if( tmpBullet->GetOwner()==1 && strstr(szTarName,"enemy") != NULL)
		{
			m_iScore++;
			m_iEnemy--;
		}

	}
	else if(strcmp(szSrcName,"myplayer")==0) //发送碰撞为我方坦克
	{
		m_pTankPlayer->OnSpriteColSprite(tarSprite);
	}
	else if(strstr(szSrcName,"enemy") != NULL)//发送碰撞为敌方坦克
	{
		CTankEnemy* tmpEnemy = (CTankEnemy*)FindWeaponByName(szSrcName);
		tmpEnemy->OnSpriteColSprite(tarSprite);
	}
}

void CGameMain::DeleteAllSprite()
{
	int n=m_vWeapon.size();
	while(m_vWeapon.size()!=0)
    {
		vector<CWeapon*>::iterator itr=m_vWeapon.begin();
		CWeapon* cw = *itr;
		m_vWeapon.erase(itr);
		cw->DeleteSprite();
		delete cw;
	}
}
