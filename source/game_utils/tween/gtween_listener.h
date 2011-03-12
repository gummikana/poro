#ifndef INC_GTWEEN_LISTENER_H
#define INC_GTWEEN_LISTENER_H

class GTween;

class GTweenListener
{
public:
	virtual ~GTweenListener() { }

	virtual void GTween_OnStep( GTween* tweener ) { }
	virtual void GTween_OnComplete( GTween* tweener ) { }
	virtual void GTween_OnStart( GTween* tweener ) { }
};


#endif
