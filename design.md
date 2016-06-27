# Data-Structures in dmClock

## ClientInfo 
a struct to hold the global config values of reservation; weight, limit togethet with their multiplicative inverses

## ReqParams 
holds rho, delta for every request.

## RequestTag 
holds tag <reservation, proportion, limit, and ready> for an individual request; constructed from either ClientInfo+ReqParams, or from scratches.

## ClientReq 
represents an individual 'request' with corresponding RequestTag 'tag' for a client 'client_id'. stored in a queue.

## ClinetRec
stands for ClientRecord, stores all 'requests' from a 'client'; stores Clientinfo/params, previous RequestTag,  .

## ClientCompare
a functor for comparing two "ClientRec" instances.

## NextReq
holds a NextReqType that indicates which 'heap' has next request, or none, or wait for a future time 

## PullReq 
	Retn {
	  C                           client;
	  typename super::RequestRef  request;
	  PhaseType                   phase;
	};
	typename super::NextReqType   type;
	boost::variant<Retn,Time>     data;


# ENUMs 

## ReadyOption 
{ignore, lowers, raises}; 

## NextReqType 
{ returning, future, none }

## HeapId 
{ reservation, ready }



# Functions
using ClientInfoFunc = std::function<ClientInfo(const C&)>;


std::map<C,ClientRecRef> client_map;

IIH:
contains ClientRec that includes a queue of ClientReq items. 