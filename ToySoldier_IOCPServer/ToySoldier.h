#pragma once


enum EControlType
{
	ECT_Human,
	ECT_AI
};

class ToySoldier : public GameObject
{
public:
	CLASS_IDENTIFICATION('TSOL', GameObject)

		enum ETSLReplicationState
	{
		ETSL_Pose = 1 << 0,
		ETSL_Color = 1 << 1,
		ETSL_PlayerId = 1 << 2,
		ETSL_Health = 1 << 3,

		ETSL_AllState = ETSL_Pose | ETSL_Color | ETSL_PlayerId | ETSL_Health
	};


	static	GameObjectPtr	StaticCreate() { return  NetworkManagerServer::sInstance->RegisterAndReturn(new ToySoldier());
	}

	virtual uint32_t GetAllStateMask()	const override { return ETSL_AllState; }

	virtual	ToySoldier*	GetAsSoldier() { return this; }

	
	virtual void HandleDying() override;


	void SetControlType(EControlType inControlType) { mControlType = inControlType; }

	void TakeDamage(int inDamagingPlayerId);

	virtual void Update()	override;

	void ProcessInput(float inDeltaTime, const InputState& inInputState);
	void SimulateMovement(float inDeltaTime);

	void ProcessCollisions();
	void ProcessCollisionsWithScreenWalls();

	void		SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; }
	uint32_t	GetPlayerId()						const { return mPlayerId; }

	void			SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
	const Vector3&	GetVelocity()						const { return mVelocity; }

	virtual uint32_t	Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

protected:
	ToySoldier();

private:



	void	AdjustVelocityByThrust(float inDeltaTime);
	void HandleShooting();


	EControlType	mControlType;


	float		mTimeOfNextShot;
	float		mTimeBetweenShots;

	Vector3				mVelocity;


	float				mMaxLinearSpeed;
	float				mMaxRotationSpeed;

	//bounce fraction when hitting various things
	float				mWallRestitution;
	float				mCatRestitution;


	uint32_t			mPlayerId;



protected:

	///move down here for padding reasons...

	float				mLastMoveTimestamp;

	float				mThrustDir;
	int					mHealth;

	bool				mIsShooting;





};

typedef shared_ptr< ToySoldier >	ToySoldierPtr;