#ifndef AliPERFORMANCETASK_H
#define AliPERFORMANCETASK_H

//------------------------------------------------------------------------------
// Task to run reconstruction performance. 
// 
// Author: J.Otwinowski 01/04/2009 
// Changes by M.Knichel 15/10/2010
//------------------------------------------------------------------------------

class AliESDEvent;
class AliESDfriend;
class AliMCEvent;
class AliPerformanceObject;
class AliMagF;
class TList;
class TTree;

#include "AliAnalysisTaskSE.h"

class AliPerformanceTask : public AliAnalysisTaskSE {
 public:
  AliPerformanceTask();
  AliPerformanceTask(const char *name, const char *title);
  virtual ~AliPerformanceTask();
  
  virtual void   UserCreateOutputObjects();
  virtual void   UserExec(Option_t *option);
  virtual void   Terminate(Option_t *);
  virtual void   FinishTaskOutput();
  virtual Bool_t Notify();

  // Add comparison objects
  Bool_t AddPerformanceObject(AliPerformanceObject* comp);

  // Use MC
  void SetUseMCInfo(Bool_t useMCInfo = kFALSE) {fUseMCInfo = useMCInfo;}

  // Use ESD friend
  void SetUseESDfriend(Bool_t useESDFriend = kFALSE) {fUseESDfriend = useESDFriend;}

  // Use HLT ESD
  void SetUseHLT(Bool_t useHLT = kFALSE) {fUseHLT = useHLT;}

  // Use Terminate function
  void SetUseTerminate(Bool_t useTerminate = kTRUE) {fUseTerminate = useTerminate;}

  // Use centrality - if yes, which one
  void  SetUseCentrality(Int_t cent)   { fUseCentrality = cent; }
  Int_t GetUseCentrality()             { return fUseCentrality; }

  // use OCDB
  void  SetUseOCDB(Bool_t bOCDB) { fUseOCDB = bOCDB;  }
  Bool_t  GetUseOCDB() { return fUseOCDB; }

  // Centrality bin to be used
  void  SetUseCentralityBin(Int_t bin) { fUseCentralityBin = bin; }
  Int_t GetUseCentralityBin()          { return fUseCentralityBin; }

 private:

  // Calculate centrality
  Int_t CalculateCentralityBin();

  AliESDEvent *fESD;          //! ESD event
  AliESDfriend *fESDfriend;   //! ESD friend event
  AliMCEvent *fMC;            //! MC event

  TList *fOutput;             //! list send on output container 1
  TTree* fOutputSummary;      //! tree to dump summary values (output container 2)
  TIterator *fPitList;        //! iterator over the output objetcs  
  TList *fCompList;           // list of comparison objects

  Bool_t fUseMCInfo;          // use MC information
  Bool_t fUseESDfriend;       // use ESD friend
  Bool_t fUseHLT;             // use HLT ESD

  Bool_t fUseTerminate;       // use terminate function

  Int_t  fUseCentrality;      // use centrality (0=off(default),1=VZERO,2=SPD)

  Bool_t  fUseOCDB;           // use OCDB

  Int_t fUseCentralityBin;  // centrality bin to be used 

  AliPerformanceTask(const AliPerformanceTask&); // not implemented
  AliPerformanceTask& operator=(const AliPerformanceTask&); // not implemented
  
  ClassDef(AliPerformanceTask, 5); // example of analysis
};

#endif
