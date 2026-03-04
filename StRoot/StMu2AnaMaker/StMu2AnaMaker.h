#ifndef StMu2AnaMaker_h
#define StMu2AnaMaker_h

#include "StMaker.h"
#include "TBranch.h"

#include "StSpinPool/StSpinDbMaker/StSpinDbMaker.h"

#include <vector>
#include <list> 

using namespace std;

class StMuDstMaker ;
class StMuDst      ;
class StMuEvent    ;
class StMuTrack    ;

class TString;
class TH1F;
class TH2F;
class TTree;

class StMu2AnaMaker : public StMaker 
{
	public:
		StMu2AnaMaker(const char* name, StMuDstMaker *muMaker, const char* outName,
							 int number_of_triggers, StSpinDbMaker* spindb);
		virtual ~StMu2AnaMaker();

		virtual Int_t Init();
		virtual Int_t InitRun(const Int_t runNumber);
		virtual Int_t Make();
		virtual void  Clear(Option_t *opt="");
		virtual Int_t Finish();

		void    addTrigger(int tr, int id) { triggers[id].push_back(tr); };

		Int_t LoadRunList();
		void  DeclareHistograms();
		void  BookTree();
		void  WriteHistograms();

		Bool_t fillEvent(StMuEvent*);
		Int_t fillTrack();
		void checkRpsScintHits(StMuDst*);
		bool RemoveTPCSector(StMuTrack* track, int sector = 5);

		Double_t rotatePhi(Double_t phi) const;

	private:
		StMuDstMaker      *mMuDstMaker;   //  Make MuDst pointer available to member functions
		StMuDst           *mMuDst;        //  MuDst pointer
		StMuEvent         *mMuEvent;      //  MuEvent pointer
		StSpinDbMaker     *mSpinDbMaker;  //  Make SpinDb pointer available to member functions

		std::vector<int>  triggers[6];   // 0-MB
		Int_t             mTriggerWord;
		Bool_t            checkTriggers(int);
		Int_t             getRunIndex(Int_t run);
		Int_t             getTrigWord();
		Bool_t            isGoodTrack(StMuTrack* mutrk);
		//--------------------------------------------
		TString           mOutName;
		TString           mOutName_hist;
		TFile             *mOutFile;
		TFile             *mOutFile_hist;
		int 	          mNumber_of_triggers;
		//--------------------------------------------
		Int_t             mMaxNRunIds;
		TString           mRunList;
		map<Int_t, Int_t> mTotalRunId;
		Int_t             runidx;
		//--------------------------------------------
		TH1F              *h_EvtCounter;
		//--------------------------------------------
		TTree             *anaTree;
		Int_t             mtrigWord;
		Float_t           mfield;
		Int_t             mrunId;
		Int_t             mnTofMatch;
		Float_t           mfillNumber;
		Float_t           mvx;
		Float_t           mvy;
		Float_t           mvz;
		Float_t           mvpdVz; 
		//--------------------------------------------
		//--------------------------------------------
		Int_t    mESpin;     // Proton's Event Spin4 value
		Int_t    mnETracks;	 // Number of Tracks in the Event

		Float_t  mtrk_px[5000];
		Float_t  mtrk_py[5000];
		Float_t  mtrk_pz[5000];
		Float_t  mtrk_Pt[5000];
		Float_t  mtrk_P[5000];
		Float_t  mtrk_Eta[5000];
		Float_t  mtrk_Phi[5000];
		Int_t    mtrk_Charge[5000];
		Float_t  mtrk_nSigmaPion[5000];
		Float_t  mtrk_nSigmaKaon[5000];
		Float_t  mtrk_nSigmaProton[5000];
		Float_t  mtrk_nSigmaElectron[5000];

		//--------------------------------------------
		ClassDef(StMu2AnaMaker, 1)
};

#endif
