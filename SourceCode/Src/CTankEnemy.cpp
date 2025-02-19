#include "CTankEnemy.h"
#include "CTankPlayer.h"
#include "LessonX.h"

CTankEnemy::CTankEnemy(const char* szName):CTankPlayer(szName) //对构造函数进行实现//本来是继承自CWeapon，但是要实现多态，所以改了改，让它继承自CTankPlayer
{
    //ctor
    m_fBulletCreateTime =   0;
    m_fSpeed            =   8;
}

CTankEnemy::~CTankEnemy()
{
    //dtor
}

void CTankEnemy::Init()
{
    int iPos = CSystem::RandomRange(0,2);
	float fPosX;
	SetDir(2);
	SetHp(2);
	switch (iPos)
	{
	case 0:
		fPosX = -24.f;
		break;
	case 1:
		fPosX = 0.f ;
		break;
    case 2:
        fPosX = 24.f;
        break;
	default:
		break;
	}
	SetSpritePosition(fPosX,-20.f);
	SetSpriteLinearVelocity(0.f,8.f);
	SetSpriteCollisionActive(1,1); //设置可以接受和发送碰撞
	SetSpriteRotation(float(90*GetDir()));
//	SetSpriteWorldLimit(WORLD_LIMIT_NULL,-26, -22, 26, 22);
}

void CTankEnemy::OnMove()
{
    int iDir = (rand() % (3 - 0 + 1)) + 0;//使用 (rand() % (b-a+1))+ a，取得 [a,b] 的随机整数
//  int iDir=0;
//	if(GetDir()==0 || GetDir()==1  || GetDir()==2)
//	{
//		iDir = GetDir()+1;
//	}
//	else //如果方向值等于3,设为0
//    {
//		iDir = 0;
//    }
	SetDir(iDir);
    float speed = GetSpeed();
    switch(GetDir())
	{
	case 0:
		SetSpeedX(0);
		SetSpeedY(-1*speed);
		break;
	case 1:
		SetSpeedX(speed);
		SetSpeedY(0);
		break;
	case 2:
		SetSpeedX(0);
		SetSpeedY(speed);
		break;
	case 3:
		SetSpeedX(-1*speed);
		SetSpeedY(0);
		break;
	}
	SetSpriteRotation(90*GetDir());
	SetSpriteLinearVelocity(GetSpeedX(),GetSpeedY());
}

void CTankEnemy::OnMove(float fDeltaTime)
{
    m_fChangeDirTime+=fDeltaTime;
	if(m_fChangeDirTime>2.0f)
	{
        OnMove();
		m_fChangeDirTime = 0.f;
	}
}

void CTankEnemy::OnFire(float fDeltaTime)
{
	m_fBulletCreateTime+=fDeltaTime;
	float Time = (rand() % (3 - 1 + 1)) + 1;//使用 (rand() % (b-a+1))+ a，取得 [a,b] 的随机整数
	//if(m_fBulletCreateTime>3.0f)
	if(m_fBulletCreateTime>Time)
	{
		m_fBulletCreateTime = 0.0f;
		float x,y;
		x = GetSpritePositionX();
		y = GetSpritePositionY();
		switch(GetDir())
		{
		case 0:
			y=y-GetSpriteHeight()/2-1;
			break;
		case 1:
			x=x+GetSpriteWidth()/2+1;
			break;
		case 2:
			y=y+GetSpriteHeight()/2+1;
			break;
		case 3:
			x=x-GetSpriteWidth()/2-1;
			break;
		}
		g_GameMain.AddBullet(GetDir(),x,y,0);
	}
}

void CTankEnemy::OnSpriteColSprite(CWeapon* pSprite)
{
	if(pSprite == NULL)
	{
		return;
	}
	SetSpriteLinearVelocity(0.f,0.f);
    m_fChangeDirTime = 1.8;
}

void CTankEnemy::TrackMove()
{
    int Dir = 0;
    CTankPlayer* m_pTankPlayer = g_GameMain.GetPalyer();
    if(GetSpritePositionX()<m_pTankPlayer->GetSpritePositionX()-2)
    {
        SetSpeedX(m_fSpeed);
        SetSpeedY(0);
        Dir = 1;
        SetDir(Dir);
    }else if(GetSpritePositionX()>m_pTankPlayer->GetSpritePositionX()+2){
        SetSpeedX(-1*m_fSpeed);
        SetSpeedY(0);
        Dir = 3;
        SetDir(Dir);
    }
    else if(GetSpritePositionY()<m_pTankPlayer->GetSpritePositionY()-2)
    {
        SetSpeedX(0);
        SetSpeedY(m_fSpeed);
        Dir = 2;
        SetDir(Dir);

    }
    else if(GetSpritePositionY()>m_pTankPlayer->GetSpritePositionY()+2){
        SetSpeedX(0);
        SetSpeedY(-1*m_fSpeed);
        Dir = 0;
        SetDir(Dir);
    }
    SetSpriteRotation(float(90*GetDir())); //用方向值乘于90得到精灵旋转度数
    SetSpriteLinearVelocity(GetSpeedX(),GetSpeedY());
}
