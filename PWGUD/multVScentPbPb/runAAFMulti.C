//
void runAAFMulti(TString dataset="/alice/sim/LHC10h8_000137161", //"/alice/sim/LHC10h6_000137161",
		 TString outFName = "trbg.root",
		 Int_t   nEvents    = -1,//3000,
		 Float_t etaMin     =-0.5,        // min eta range to fill in histos
		 Float_t etaMax     = 0.5,        // max eta range to fill in histos
		 Float_t zMin       = -7,         // process events with Z vertex min
		 Float_t zMax       =  7,         //                     max positions
		 Char_t* useCentVar = 0,          // centrality variable to use
		 Float_t scaleMCV0  = 1.0,     // rescale MC V0 to match data
		 //
		 //
		 Float_t cutSigNStd  = 1.5,       // cut on weighed distance used to extract signal
		 Float_t cutSigDPhiS = -1,        // cut on dPhi-phiBent used to extract signal (if negative -> dphi*sqrt(cutSigNStd)
		 Bool_t  useMC  = kTRUE,          // fill MC info (doRec=kTRUE)
		 //
		 Bool_t doRec  = kFALSE,//kTRUE,           // fill data histos from new reco
		 Bool_t doInj  = kFALSE,//kTRUE,           // create Inj. bg
		 Bool_t doRot  = kFALSE,          // create Rot. bg
		 Bool_t doMix  = kFALSE,//kTRUE,  // create Mix. bg
		 // 
		 // specific parameters for reconstruction
		 float  phiRot      = 3.14159e+00, // angle for bg. generation with rotation
		 float  injScale    = 1.,//0.7,    // inject injScale*Ncl(Lr1/Lr2) hits
		 Bool_t scaleDTheta = kTRUE,       // scale dTheta by 1/sin^2(theta) in trackleting
		 float  nStdDev     = 25.,         // number of st.dev. for tracklet cut to keep
		 float  dphi        = 0.06,        // dphi window (sigma of tracklet cut)
		 float  dtht        = 0.025,       // dtheta .... (if negative, abs will be used with additional cut on |dthetaX|, apart from w.distance
		 float  phishift    = 0.0045,      // bending shift
		 Bool_t remOvl      = kTRUE,       
		 float  ovlPhiCut   = 0.005, 
		 float  ovlZetaCut  = 0.05,
		 Int_t  nEventsSkip = 0,
		 //----------------------- Ntracklets selection parameters important for mixing, to be tuned
		 Float_t ntMin      =   1,         // process events with ESDmult 
		 Float_t ntMax      = 20000,       // within this range
		 Float_t ntMixBinSz = 20000,       // ESDMult bin size for mixing
		 //----------------------- Zv selection parameters important for mixing, to be tuned
		 Float_t zMixBinSz  =  14,       //0.1,  // Zv. bin for mixing
		 //---------------------------------------------------------------------------------
		 //
		 Bool_t checkReconstructables = kFALSE,//kTRUE, // fill histos for reconstructable (needs useMC and doRec) 
		 //
		 TString alirootVer = "VO_ALICE@AliRoot::vAN-20140409",
		 //TString rootVer    = "VO_ALICE@ROOT::v5-33-02b",
		 TString rootVer    = "VO_ALICE@ROOT::v5-34-08-6",
		 //
		 //TString proofCluster="shahoian@skaf.saske.sk"
		 TString proofCluster="shahoian@alice-caf.cern.ch"
		 ) 
{ 
  //  
  TString aliver = gSystem->Getenv("ANTAG");
  if (!aliver.IsNull()) alirootVer = "VO_ALICE@AliRoot::"+aliver;
  TString roover = gSystem->Getenv("ROOTVER");
  if (!aliver.IsNull()) rootVer = "VO_ALICE@ROOT::"+roover;
  //
  printf("Use %s %s CentVar = %s\n",alirootVer.Data(), rootVer.Data(), useCentVar);
//  return;
  printf("Mode: %s %s C(%f %d) %d %d %d %d | %f\n",dataset.Data(), outFName.Data(), cutSigDPhiS,useMC, doRec,doInj,doRot,doMix,phiRot);
  Bool_t runLocal = kFALSE;//kTRUE; // true only for local test mode
  if (runLocal) {
    //    dataset = "/default/shahoian/test_pp";//"/default/shahoian/test";
    //    dataset = "/default/shahoian/pA188359";
    proofCluster = "";
    alirootVer = "$ALICE_ROOT/ANALYSIS/macros/AliRootProofLite";
    //useMC = kFALSE;
    gSystem->ExpandPathName(alirootVer);
    //    nEvents = 500;
  }
  //
  /*
  if ((!dataset.Contains("alice/sim")) && useMC) {
    printf("Running with read data dataset, switching OFF useMC\n");
    useMC = kFALSE;
  }
  */
  //
  printf("Requested: %s %s\n",alirootVer.Data(), rootVer.Data());
  printf("Output expected in %s\n",outFName.Data());
  //
  gEnv->SetValue("XSec.GSI.DelegProxy","2");
  //
  TString alirootMode="REC";
  //  TString extraLibs = "ITSrec:CDB:Geom:"; // not needed in default aliroot mode
  TString extraLibs = "Physics:Minuit:Proof:Geom:STEERBase:ESD:CDB:RAWDatabase:RAWDatarec:AOD:ANALYSIS:"
    "STEER:STRUCT:ITSbase:ITSrec:OADB:ANALYSISalice";
 // not needed in default aliroot mode

  //extraLibs+= "ANALYSIS:ANALYSISalice";
  //  extraLibs+= "ANALYSIS:OADB:ANALYSISalice:EventMixing";
  TList *list = new TList();
  // sets $ALIROOT_MODE on each worker to let proof to know to run in special mode
  list->Add(new TNamed("ALIROOT_MODE"      , alirootMode.Data()));
  list->Add(new TNamed("ALIROOT_EXTRA_LIBS", extraLibs.Data()));
  list->Add(new TNamed("ALIROOT_EXTRA_INCLUDES", "ITS:include"));
  if (doRec || doInj || doRot || doMix) list->Add(new TNamed("ALIROOT_ENABLE_ALIEN","1"));
  //
  //REM: same version of AliRoot on client!!!!! Otherwise error!! 
  TProof::Mgr(proofCluster.Data())->SetROOTVersion(rootVer.Data());
  TProof::Open(proofCluster.Data());//,"workers=1x");
  if (runLocal) gProof->SetParallel(4);
  if (!gProof) {
    Error("runAAFMulti.C","Connection to AF failed.");
    return;
  }
  //  gProof->Exec("TObject *o = gEnv->GetTable()->FindObject(\"Proof.UseMergers\");"
  //	       "gEnv->GetTable()->Remove(o);", kTRUE);
  gProof->SetParameter("PROOF_UseMergers", 0);
  // Lets enable aliroot + extra libs on proof cluster
  if (runLocal) gProof->UploadPackage(alirootVer.Data());
  if ( gProof->EnablePackage(alirootVer.Data(), list) ) {
    printf("Failed to enable packages\n");
    return;
  }
  //  gProof->EnablePackage(alirootVer.Data());
  //
  if (runLocal) {
    Int_t numWorkers = gProof->GetParallel();
    if (numWorkers<1) {printf("No workers\n"); return;}
    gProof->SetParameter("PROOF_PacketizerStrategy", (Int_t)0);
    int frac = (Int_t) 5 / numWorkers;
    if (frac<1) frac = 1;
    gProof->SetParameter("PROOF_PacketAsAFraction", frac);
  }
  //
  gROOT->LoadMacro("MyAnalysisMacroTrackletMulti.C");
  TStopwatch sw;
  sw.Start();
  printf("Mode: %d %d %d %d\n",doRec,doInj,doRot,doMix);
  MyAnalysisMacroTrackletMulti(dataset,outFName,nEvents,etaMin,etaMax,zMin,zMax,useCentVar,
			       cutSigNStd,cutSigDPhiS,useMC,scaleMCV0,
			       doRec,doInj,doRot,doMix,
			       phiRot,injScale,scaleDTheta,nStdDev,dphi,dtht,
			       phishift,remOvl,ovlPhiCut,ovlZetaCut,nEventsSkip,
			       ntMin,ntMax,ntMixBinSz,zMixBinSz,
			       checkReconstructables, runLocal);
  //
  sw.Stop();
  sw.Print();
}
