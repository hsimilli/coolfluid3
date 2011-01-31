// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

// this file is en-block included into utest-parallel-collective.cpp
// do not include anything here, rather in utest-parallel-collective.cpp

////////////////////////////////////////////////////////////////////////////////

struct PEAllReduceFixture
{
  /// common setup for each test case
  PEAllReduceFixture()
  {
    // rank and proc
    nproc=mpi::PE::instance().size();
    irank=mpi::PE::instance().rank();

    // ptr helpers
    sndcnt=0;
    rcvcnt=0;
    ptr_sndcnt=new int[nproc];
    ptr_rcvcnt=new int[nproc];
    ptr_snddat=new double[nproc*nproc];
    ptr_rcvdat=new double[nproc*nproc];
    ptr_sndmap=new int[nproc*nproc];
    ptr_rcvmap=new int[nproc*nproc];
    ptr_tmprcv=new double[nproc*nproc];
    ptr_tmpcnt=new int[nproc];

    // std::Vector helpers
    vec_sndcnt.resize(nproc);
    vec_rcvcnt.resize(nproc);
    vec_snddat.resize(nproc*nproc);
    vec_rcvdat.resize(nproc*nproc);
    vec_sndmap.resize(nproc*nproc);
    vec_rcvmap.resize(nproc*nproc);
    vec_tmprcv.resize(0);
    vec_tmpcnt.resize(nproc);
    vec_tmprcvchr.resize(nproc*nproc*sizeof(double));
    vec_snddatchr.resize(nproc*nproc*sizeof(double));
  }

  /// common tear-down for each test case
  ~PEAllReduceFixture()
  {
    delete[] ptr_sndcnt;
    delete[] ptr_rcvcnt;
    delete[] ptr_snddat;
    delete[] ptr_rcvdat;
    delete[] ptr_sndmap;
    delete[] ptr_rcvmap;
    delete[] ptr_tmprcv;
    delete[] ptr_tmpcnt;
  }

  /// number of processes
  int nproc;
  /// rank of process
  int irank;

  /// data for raw pointers
  int     sndcnt;
  int     rcvcnt;
  int*    ptr_sndcnt;
  int*    ptr_rcvcnt;
  double* ptr_snddat;
  double* ptr_rcvdat;
  int*    ptr_sndmap;
  int*    ptr_rcvmap;
  double* ptr_tmprcv;
  int*    ptr_tmpcnt;

  /// data for std::vectors
  std::vector<int>    vec_sndcnt;
  std::vector<int>    vec_rcvcnt;
  std::vector<double> vec_snddat;
  std::vector<double> vec_rcvdat;
  std::vector<int>    vec_sndmap;
  std::vector<int>    vec_rcvmap;
  std::vector<double> vec_tmprcv;
  std::vector<int>    vec_tmpcnt;
  std::vector<char>   vec_tmprcvchr;
  std::vector<char>   vec_snddatchr;

  /// helper function for constant size data - setting up input and verification data
  void setup_data_constant()
  {
    int i,j;
    for (i=0; i<nproc; i++)
      for (j=0; j<nproc; j++){
        ptr_snddat[i*nproc+j]=(irank+1)*1000000+(i+1)*10000+(j+1);
        ptr_rcvdat[i*nproc+j]=(i+1)*1000000+(irank+1)*10000+(j+1);
      }
    sndcnt=nproc*nproc;
    rcvcnt=nproc*nproc;
    vec_snddat.assign(ptr_snddat,ptr_snddat+nproc*nproc);
    vec_rcvdat.assign(ptr_rcvdat,ptr_rcvdat+nproc*nproc);
    vec_snddatchr.assign((char*)(ptr_snddat),(char*)(ptr_snddat+nproc*nproc));
  }

  /// helper function for variable size data - setting up input and verification data
  void setup_data_variable()
  {
    int i,j,k,l;
    for (i=0; i<nproc; i++){
      ptr_sndcnt[i]=(i+irank*irank)%nproc;
      ptr_rcvcnt[i]=(i*i+irank)%nproc;
    }
    for(i=0; i<nproc*nproc; i++) { // making debugger shut up for uninitialized values
      ptr_snddat[i]=0.;
      ptr_rcvdat[i]=0.;
      ptr_sndmap[i]=0;
      ptr_rcvmap[i]=0;
    }
    for(i=0, k=0; i<nproc; i++)
      for(j=0; j<ptr_sndcnt[i]; j++, k++)
        ptr_snddat[k]=(irank+1)*1000000+(i+1)*10000+(j+1);
    for(i=0, k=0; i<nproc; i++)
      for(j=0; j<ptr_rcvcnt[i]; j++, k++)
        ptr_rcvdat[k]=(i+1)*1000000+(irank+1)*10000+(j+1);
    for (i=0, k=0,l=0; i<nproc; k+=ptr_sndcnt[i], i++)
      for (j=0; j<ptr_sndcnt[i]; j++,l++)
        ptr_sndmap[l]=k+ptr_sndcnt[i]-1-j; // flipping all sets for each process
    for (i=0, k=0, l=0; i<nproc; k+=ptr_rcvcnt[i], i++)
      for (j=0; j<ptr_rcvcnt[i]; j++, l++)
        ptr_rcvmap[l]=i*nproc+ptr_rcvcnt[i]-1-j; // redirecting to align start with nproc numbers
    for(i=0, sndcnt=0, rcvcnt=0; i<nproc; i++){
      sndcnt+=ptr_sndcnt[i];
      rcvcnt+=ptr_rcvcnt[i];
    }
    vec_sndcnt.assign(ptr_sndcnt,ptr_sndcnt+nproc);
    vec_rcvcnt.assign(ptr_rcvcnt,ptr_rcvcnt+nproc);
    vec_snddat.assign(ptr_snddat,ptr_snddat+nproc*nproc);
    vec_rcvdat.assign(ptr_rcvdat,ptr_rcvdat+nproc*nproc);
    vec_sndmap.assign(ptr_sndmap,ptr_sndmap+nproc*nproc);
    vec_rcvmap.assign(ptr_rcvmap,ptr_rcvmap+nproc*nproc);
    vec_snddatchr.assign((char*)(ptr_snddat),(char*)(ptr_snddat+nproc*nproc));
  }

  // test class with operator + to test if operations and all_reduce can work with it
  class optest {
    public:
      // simple data
      int ival;
      double dval;
      // constructor for giving values for i and d
      optest(){
        ival=mpi::PE::instance().rank()+1;
        dval=(double)mpi::PE::instance().rank()+10.;
      }
      // operator +
      optest operator +(const optest& b) const { optest t; t.ival=ival+b.ival; t.dval=dval+b.dval; return t; };
      // function to test result
      bool test()
      {
        int i;
        int nproc=mpi::PE::instance().rank();
        int itest=0;
        for(i=0; i<nproc; i++) itest+=i+1;
        int dtest=0;
        for(i=0; i<nproc; i++) dtest+=(double)i+10.;
        return ((ival==itest)&&(dval==dtest));
      }
  };


};

////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( PEAllReduceSuite, PEAllReduceFixture )

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce )
{
  PEProcessSortedExecute(mpi::PE::instance(),-1,CFinfo << "Testing all_reduce " << irank << "/" << nproc << CFendl; );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_most_common_ops )
{
  int i;
  int ival,itest,iresult;
  double dval,dtest,dresult;
  ival=irank+1;
  dval=(double)irank+1.;

  // testing plus
  itest=0;
  dtest=0.;
  for(i=0; i<nproc; i++) { itest+=i+1; dtest+=(double)i+1.; }
  iresult=-1;
  dresult=-1.;
  mpi::all_reduce(mpi::PE::instance(), mpi::plus(), &ival, 1, &iresult);
  BOOST_CHECK_EQUAL( iresult, itest );
  mpi::all_reduce(mpi::PE::instance(), mpi::plus(), &dval, 1, &dresult);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing multiplies
  itest=1;
  dtest=1.;
  for(i=0; i<nproc; i++) { itest*=i+1; dtest*=(double)i+1.; }
  iresult=-1;
  dresult=-1.;
  mpi::all_reduce(mpi::PE::instance(), mpi::multiplies(), &ival, 1, &iresult);
  BOOST_CHECK_EQUAL( iresult, itest );
  mpi::all_reduce(mpi::PE::instance(), mpi::multiplies(), &dval, 1, &dresult);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing max
  itest=nproc;
  dtest=(double)nproc;
  iresult=-1;
  dresult=-1.;
  mpi::all_reduce(mpi::PE::instance(), mpi::max(), &ival, 1, &iresult);
  BOOST_CHECK_EQUAL( iresult, itest );
  mpi::all_reduce(mpi::PE::instance(), mpi::max(), &dval, 1, &dresult);
  BOOST_CHECK_EQUAL( dresult, dtest );

  // testing max
  itest=1;
  dtest=1.;
  iresult=-1;
  dresult=-1.;
  mpi::all_reduce(mpi::PE::instance(), mpi::min(), &ival, 1, &iresult);
  BOOST_CHECK_EQUAL( iresult, itest );
  mpi::all_reduce(mpi::PE::instance(), mpi::min(), &dval, 1, &dresult);
  BOOST_CHECK_EQUAL( dresult, dtest );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_operator_of_class )
{
/*
  optest in,out;
  mpi::all_reduce(mpi::PE::instance(), mpi::plus(), &in, 1, &out);
  PEProcessSortedExecute(mpi::PE::instance(),-1,CFinfo << nproc << " results: " << out.ival << " " << out.dval << CFendl;);
  BOOST_CHECK_EQUAL( out.test() , true );
*/
}

/*
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_ptr_constant )
{
  int i;

  setup_data_constant();

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;

  ptr_tmprcv=mpi::all_reduce(mpi::PE::instance(), ptr_snddat, nproc, (double*)0);
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=0.;
  mpi::all_reduce(mpi::PE::instance(), ptr_snddat, nproc, ptr_tmprcv);
  for (i=0; i<nproc*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), ptr_tmprcv, nproc, ptr_tmprcv);
  for (i=0; i<nproc*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  ptr_tmprcv=(double*)mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, nproc, (char*)0, sizeof(double));
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=0.;
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, nproc, (char*)ptr_tmprcv, sizeof(double));
  for (i=0; i<nproc*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );

  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_tmprcv, nproc, (char*)ptr_tmprcv, sizeof(double));
  for (i=0; i<nproc*nproc; i++)  BOOST_CHECK_EQUAL( ptr_tmprcv[i] , ptr_rcvdat[i] );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_vector_constant )
{
  int i;

  setup_data_constant();

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_tmprcv);
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );
  BOOST_CHECK_EQUAL( (int)vec_tmprcv.size() , rcvcnt );

  vec_tmprcv.assign(nproc*nproc,0.);
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_tmprcv);
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );

  vec_tmprcv=vec_snddat;
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcv, vec_tmprcv);
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( vec_tmprcv[i] , vec_rcvdat[i] );

  vec_tmprcvchr.resize(0);
  vec_tmprcvchr.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_tmprcvchr );
  BOOST_CHECK_EQUAL( vec_tmprcvchr.size() , sizeof(double)*rcvcnt );
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i], vec_rcvdat[i] );

  for (i=0; i<nproc*nproc; i++) ((double*)(&vec_tmprcvchr[0]))[i]=0.;
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_tmprcvchr );
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i], vec_rcvdat[i] );

  vec_tmprcvchr.assign((char*)(ptr_snddat),(char*)(ptr_snddat+nproc*nproc));
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcvchr, vec_tmprcvchr );
  for (i=0; i<nproc*nproc; i++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i], vec_rcvdat[i] );
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_ptr_variable )
{
  int i,j,k;

  setup_data_variable();

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  ptr_tmprcv=mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, (double*)0, ptr_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[k]=0.;
  mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, ptr_tmprcv, ptr_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), ptr_tmprcv, ptr_sndcnt, ptr_tmprcv, ptr_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  ptr_tmprcv=mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, (double*)0, ptr_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[k]=0.;
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, ptr_tmprcv, ptr_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), ptr_tmprcv, ptr_sndcnt, ptr_tmprcv, ptr_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  ptr_tmprcv=mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, ptr_sndmap, (double*)0, ptr_rcvcnt, ptr_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[i*nproc+j]=0.;
  mpi::all_reduce(mpi::PE::instance(), ptr_snddat, ptr_sndcnt, ptr_sndmap, ptr_tmprcv, ptr_rcvcnt, ptr_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), ptr_tmprcv, ptr_sndcnt, ptr_sndmap, ptr_tmprcv, ptr_rcvcnt, ptr_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  ptr_tmprcv=(double*)mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, (char*)0, ptr_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[k]=0.;
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, (char*)ptr_tmprcv, ptr_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_tmprcv, ptr_sndcnt, (char*)ptr_tmprcv, ptr_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  ptr_tmprcv=(double*)mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, (char*)0, ptr_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[k]=0.;
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, (char*)ptr_tmprcv, ptr_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  for(i=0; i<nproc; i++) ptr_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_tmprcv, ptr_sndcnt, (char*)ptr_tmprcv, ptr_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( ptr_tmpcnt[i] , ptr_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[k] , ptr_rcvdat[k] );

  delete[] ptr_tmprcv;
  ptr_tmprcv=0;
  ptr_tmprcv=(double*)mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, ptr_sndmap, (char*)0, ptr_rcvcnt, ptr_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) ptr_tmprcv[i*nproc+j]=0.;
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_snddat, ptr_sndcnt, ptr_sndmap, (char*)ptr_tmprcv, ptr_rcvcnt, ptr_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  delete[] ptr_tmprcv;
  ptr_tmprcv=new double[nproc*nproc];
  for (i=0; i<nproc*nproc; i++) ptr_tmprcv[i]=ptr_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), (char*)ptr_tmprcv, ptr_sndcnt, ptr_sndmap, (char*)ptr_tmprcv, ptr_rcvcnt, ptr_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<ptr_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ptr_tmprcv[i*nproc+j] , ptr_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( all_reduce_vector_variable )
{
  int i,j,k;

  setup_data_variable();

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_tmprcv, vec_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) vec_tmprcv[k]=0.;
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_tmprcv, vec_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  vec_tmprcv.resize(nproc*nproc);
  vec_tmprcv.reserve(nproc*nproc);
  for (i=0; i<nproc*nproc; i++) vec_tmprcv[i]=vec_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcv, vec_sndcnt, vec_tmprcv, vec_rcvcnt);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_tmprcv, vec_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) vec_tmprcv[k]=0.;
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_tmprcv, vec_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  vec_tmprcv.resize(nproc*nproc);
  vec_tmprcv.reserve(nproc*nproc);
  for (i=0; i<nproc*nproc; i++) vec_tmprcv[i]=vec_snddat[i];
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcv, vec_sndcnt, vec_tmprcv, vec_tmpcnt);
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[k] , vec_rcvdat[k] );

  vec_tmprcv.resize(0);
  vec_tmprcv.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_sndmap, vec_tmprcv, vec_rcvcnt, vec_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) vec_tmprcv[i*nproc+j]=0.;
  mpi::all_reduce(mpi::PE::instance(), vec_snddat, vec_sndcnt, vec_sndmap, vec_tmprcv, vec_rcvcnt, vec_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  vec_tmprcv.resize(nproc*nproc);
  vec_tmprcv.reserve(nproc*nproc);
  for (i=0; i<nproc*nproc; i++) vec_tmprcv[i]=vec_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcv, vec_sndcnt, vec_sndmap, vec_tmprcv, vec_rcvcnt, vec_rcvmap);
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( vec_tmprcv[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  vec_tmprcvchr.resize(0);
  vec_tmprcvchr.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_tmprcvchr, vec_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) ((double*)(&vec_tmprcvchr[0]))[k]=0.;
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_tmprcvchr, vec_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  vec_tmprcvchr.resize(nproc*nproc*sizeof(double));
  vec_tmprcvchr.reserve(nproc*nproc*sizeof(double));
  for (i=0; i<nproc*nproc; i++) ((double*)(&vec_tmprcvchr[0]))[i]=vec_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcvchr, vec_sndcnt, vec_tmprcvchr, vec_rcvcnt, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  vec_tmprcvchr.resize(0);
  vec_tmprcvchr.reserve(0);
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_tmprcvchr, vec_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) ((double*)(&vec_tmprcvchr[0]))[k]=0.;
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_tmprcvchr, vec_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  vec_tmprcvchr.resize(nproc*nproc*sizeof(double));
  vec_tmprcvchr.reserve(nproc*nproc*sizeof(double));
  for (i=0; i<nproc*nproc; i++) ((double*)(&vec_tmprcvchr[0]))[i]=vec_snddat[i];
  for(i=0; i<nproc; i++) vec_tmpcnt[i]=-1;
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcvchr, vec_sndcnt, vec_tmprcvchr, vec_tmpcnt, sizeof(double));
  for (i=0; i<nproc; i++) BOOST_CHECK_EQUAL( vec_tmpcnt[i] , vec_rcvcnt[i] );
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[k] , vec_rcvdat[k] );

  vec_tmprcvchr.resize(0);
  vec_tmprcvchr.reserve(0);
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_sndmap, vec_tmprcvchr, vec_rcvcnt, vec_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) vec_tmprcvchr[i*nproc+j]=0.;
  mpi::all_reduce(mpi::PE::instance(), vec_snddatchr, vec_sndcnt, vec_sndmap, vec_tmprcvchr, vec_rcvcnt, vec_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

  vec_tmprcvchr.resize(nproc*nproc*sizeof(double));
  vec_tmprcvchr.reserve(nproc*nproc*sizeof(double));
  for (i=0; i<nproc*nproc; i++) ((double*)(&vec_tmprcvchr[0]))[i]=vec_snddat[i];
  mpi::all_reduce(mpi::PE::instance(), vec_tmprcvchr, vec_sndcnt, vec_sndmap, vec_tmprcvchr, vec_rcvcnt, vec_rcvmap, sizeof(double));
  for (i=0, k=0; i<nproc; i++) for (j=0; j<vec_rcvcnt[i]; j++, k++) BOOST_CHECK_EQUAL( ((double*)(&vec_tmprcvchr[0]))[i*nproc+j] , vec_rcvdat[k] ); // i*nproc+j is not a bug, check reason at init

}
*/
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
