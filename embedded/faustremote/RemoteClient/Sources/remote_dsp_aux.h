/************************************************************************
 ************************************************************************
 FAUST compiler
 Copyright (C) 2003-2013 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************
 ************************************************************************/

#ifndef remote_dsp_aux__h
#define remote_dsp_aux__h

#include <string>
#include <vector>
#include <map>
#include <assert.h>

#include <sstream>
#include <iostream>
#include <fstream> 

#include "faust/gui/UI.h"
#include "faust/gui/meta.h"
#include "faust/audio/dsp.h"

#include <dns_sd.h>

#include "jack/net.h"
#include <curl/curl.h>

#include "JsonParser.h"

#define	EXPORT __attribute__ ((visibility("default")))

#include <vector>

#ifdef __cplusplus
extern "C"
{
#endif

using namespace std;

class remote_dsp_aux;

// Standard Callback to store a server response in strinstream
static size_t      store_Response(void *buf, size_t size, size_t nmemb, void* userp);
    
class remote_dsp_factory{
    
private:
    
    string      fIndex;             //Unique Index to bind a Remote_Factory to its llvm_Factory on the server side
    
    int         fNumInputs;
    int         fNumOutputs;        //Num of In/Output of compiled DSP factory

    string      fServerIP;          //IP of remote server 
    
    map<string,string>  fMetadatas; //Metadatas extracted from json
    vector<itemInfo*>   fUiItems;   //Items extracted from json
    
    void        decodeJson(const string& json);
    
public: 

    remote_dsp_aux* createRemoteDSPInstance(int argc, const char *argv[], int samplingRate, int bufferSize, string& error);

    bool        init(int argc, const char *argv[], const string& ipServer, int portServer, const string& nameApp, string dspContent, string& error, int opt_level);
    void        stop();
    
    void         metadataRemoteDSPFactory(Meta* m);  
    
//    ACCESSORS
    string              serverIP(){return fServerIP;}
    vector<itemInfo*>   itemList(){return fUiItems;}
    int                 numInputs(){return fNumInputs;}
    int                 numOutputs(){return fNumOutputs;}
    string              index(){return fIndex;}
    
};
    
//---------------------- Public C++ interface
    
EXPORT remote_dsp_factory* createRemoteDSPFactoryFromFile(const string& filename, int argc, const char *argv[], const string& ipServer, int portServer, string& error, int opt_level);
    
EXPORT remote_dsp_factory* createRemoteDSPFactoryFromString(const string& name_app, const string& dsp_content, int argc, const char *argv[], const string& ipServer, int portServer, string& error, int opt_level);
    
EXPORT void deleteRemoteDSPFactory(remote_dsp_factory* factory);
 
EXPORT void metadataRemoteDSPFactory(remote_dsp_factory* factory, Meta* m);
    
//------
    
class remote_dsp_aux : public dsp{

    private:
    
        int                     fBufferSize;        //Buffer Size of NetJack connection   
    
        remote_dsp_factory*     fFactory;           //Factory is it created from
        
        jack_net_master_t*      fNetJack;           //Jack Connection
    
        float**                 fInputs;            //Concatenation of control buffer & audio buffer
        float**                 fOutputs;           //Concatenation of control buffer & audio buffer

        FAUSTFLOAT*             fOutControl;        //Buffer containing the values of controls
        FAUSTFLOAT*             fInControl;         //Buffer containing the values of controls

        void fillBufferWithZeros(int size1, int size2, FAUSTFLOAT** buffer);
        void setupBuffers(FAUSTFLOAT** input, FAUSTFLOAT** output, int offset);
    
//    Command-line parsing fonction
        const char*  getValueFromKey(int argc, const char *argv[], const char *key, const char* defaultValue);  
        
    public:   
    
        remote_dsp_aux(remote_dsp_factory* factory);
        ~remote_dsp_aux();
    
        virtual int     getNumInputs();
        virtual int     getNumOutputs();

        virtual void    init(int samplingFreq);
    
        virtual void    buildUserInterface(UI* ui);
    
        virtual void    compute(int count, FAUSTFLOAT** input, FAUSTFLOAT** output);
    
        bool            init(int argc, const char *argv[], int samplingFreq, int buffer_size, string& error);
};
    
class EXPORT remote_dsp : public dsp{
    
public: 
    
    virtual int     getNumInputs();
    virtual int     getNumOutputs();
    
    virtual void    init(int samplingFreq);
    
    virtual void    buildUserInterface(UI* ui);
    
    virtual void    compute(int count, FAUSTFLOAT** input, FAUSTFLOAT** output);
};

EXPORT remote_dsp*  createRemoteDSPInstance(remote_dsp_factory* factory, int argc, const char *argv[], int samplingRate, int bufferSize, string& error);

EXPORT void deleteRemoteDSPInstance(remote_dsp* dsp);
    
EXPORT bool getRemoteMachinesAvailable(map<string, pair<string,int> >* machineList);

static void browsingCallback(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *regtype, const char *replyDomain, void *context );

#ifdef __cplusplus
}
#endif
    
#endif