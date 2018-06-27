//#include <algorithm>
#include <boost/algorithm/string/replace.hpp>

#include "net.h"
#include "masternodeconfig.h"
#include "util.h"
#include "chainparams.h"
#include "walletdb.h"
#include "wallet.h"
#include "base58.h"
#include "init.h"
#include "main.h"
#include "ui_interface.h"

CMasternodeConfig masternodeConfig;

void CMasternodeConfig::add(std::string alias, std::string ip, std::string privKey, std::string txHash, std::string outputIndex) 
{
    CMasternodeEntry cme(alias, ip, privKey, txHash, outputIndex);
    entries.push_back(cme);
}

void CMasternodeConfig::remove(std::string ip)
{
    int i = 0;
    BOOST_FOREACH(CMasternodeEntry adrenaline, entries)
    {
        if(adrenaline.getIp() == ip) {
            entries.erase(entries.begin() + i);
        }
        i++;
    }
}

bool CMasternodeConfig::read(std::string& strErr) 
{
    
    int linenumber = 1;
    boost::filesystem::path pathMasternodeConfigFile = GetMasternodeConfigFile();
    boost::filesystem::ifstream streamConfig(pathMasternodeConfigFile);

    std::string strWalletFile = GetArg("-wallet", "wallet.dat");
    boost::filesystem::path pathWallet = GetDataDir() / strWalletFile;

    if (!streamConfig.good()) 
    {
        FILE* configFile = fopen(pathMasternodeConfigFile.string().c_str(), "a");
        if (configFile != NULL) 
        {
            std::string strHeader = "# Masternode config file\n"
                                    "# Format: alias IP:port masternodeprivkey collateral_output_txid collateral_output_index\n"
                                    "# Example: mn1 127.0.0.2:26868 93HaYBVUCYjEMeeH1Y4sBGLALQZE1Yc1K64xiqgX37tGBDQL8Xg 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a67c 0\n";
            fwrite(strHeader.c_str(), std::strlen(strHeader.c_str()), 1, configFile);
            fclose(configFile);
        }

//        std::ifstream walletDat(pathWallet.string().c_str(), std::ios::binary);

//        if(!walletDat.good()) {
//            std::ofstream newWalletFile(pathWallet.string().c_str(), std::ios::binary);
//            newWalletFile.close();
//        }

        return true; // Nothing to read, so just return
    }

    pwalletMain = new CWallet(pathWallet.string().c_str());

    CWalletDB walletdb(pwalletMain->strWalletFile);
	
    for (std::string line; std::getline(streamConfig, line); linenumber++) 
    {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string comment, alias, ip, privKey, txHash, outputIndex;

        if (iss >> comment) {
            if (comment.at(0) == '#') continue;
            iss.str(line);
            iss.clear();
        }

        if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex))
        {
            iss.str(line);
            iss.clear();
            if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex))
            {
                LogPrintf("CMasternodeConfig::read - Could not parse masternode.conf. Line: %s\n", line.c_str());
                streamConfig.close();
                return false;
            }
        }

        CBitCannaNodeConfig c;

        boost::replace_all(alias, "_", " ");
        c.sAlias = alias;
        c.sAddress = ip;
        c.sMasternodePrivKey = privKey;

        CAccount account;
        walletdb.ReadAccount(c.sAlias, account);
        bool bKeyUsed = false;
        bool bForceNew = false;

        // Check if the current key has been used
        if (account.vchPubKey.IsValid())
        {
            CScript scriptPubKey = GetScriptForDestination(account.vchPubKey.GetID());
            for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin();
                 it != pwalletMain->mapWallet.end() && account.vchPubKey.IsValid();
                 ++it)
            {
                const CWalletTx& wtx = (*it).second;
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                    if (txout.scriptPubKey == scriptPubKey)
                        bKeyUsed = true;
            }
        }

        // Generate a new key
        if (!account.vchPubKey.IsValid() || bForceNew || bKeyUsed)
        {
            if (!pwalletMain->GetKeyFromPool(account.vchPubKey))
            {
                LogPrintf("Keypool ran out, please call keypoolrefill first.");
                streamConfig.close();
                return false;
            }
            pwalletMain->SetAddressBook(account.vchPubKey.GetID(), c.sAlias, "");
            walletdb.WriteAccount(c.sAlias, account);
        }

        c.sCollateralAddress = CBitcoinAddress(account.vchPubKey.GetID()).ToString();

        pwalletMain->mapMyBitCannaNodes.insert(make_pair(c.sAddress, c));
        walletdb.WriteBitCannaNodeConfig(c.sAddress, c);
        uiInterface.NotifyBitCannaNodeChanged(c);

        if (!(CService(ip).IsIPv4() && CService(ip).IsRoutable())) 
        {
            LogPrintf("Invalid Address detected in masternode.conf: %s (IPV4 ONLY) \n", line.c_str());
            streamConfig.close();
            return false;
        }

        add(alias, ip, privKey, txHash, outputIndex);
    }
    streamConfig.close();
    return true;
}

bool CMasternodeConfig::write()
{
    boost::filesystem::path pathMasternodeConfigFile = GetMasternodeConfigFile();
    boost::filesystem::ofstream streamConfig(pathMasternodeConfigFile);

    if(pwalletMain)
    {
        string alias;
//        LOCK(cs_adrenaline);
        BOOST_FOREACH(CMasternodeEntry adrenaline, getEntries())
        {
            alias = adrenaline.getAlias();
            boost::replace_all(alias, " ", "_");
            streamConfig << alias << " " << adrenaline.getIp() << " " << adrenaline.getPrivKey() << " " << adrenaline.getTxHash() << " " << adrenaline.getOutputIndex() << "\n";
        }
    }

    streamConfig.close();
    return true;
}

bool CMasternodeConfig::CMasternodeEntry::castOutputIndex(int &n)
{
    try {
        n = std::stoi(outputIndex);
    } catch (const std::exception e) {
        LogPrintf("%s: %s on getOutputIndex\n", __func__, e.what());
        return false;
    }

    return true;
}
