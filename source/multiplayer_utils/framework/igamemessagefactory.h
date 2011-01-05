#ifndef INC_IGAMEMESSAGEFACTORY_H
#define INC_IGAMEMESSAGEFACTORY_H

class IGameMessage;

class IGameMessageFactory
{
public:

	virtual ~IGameMessageFactory() { }

	virtual IGameMessage* GetNewMessage( int type )  = 0;

	virtual int GetGameMessageID_First() const = 0;
	virtual int GetGameMessageID_Last() const = 0;
};

#endif

