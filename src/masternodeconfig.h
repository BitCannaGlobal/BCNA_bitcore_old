// Copyright (c) 2009-2010 Satoshi Nakamoto             -*- c++ -*-
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SRC_MASTERNODECONFIG_H_
#define SRC_MASTERNODECONFIG_H_

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../primitives/transaction.h"

class CMasternodeConfig;
extern CMasternodeConfig masternodeConfig;

class CMasternodeConfig
{

public:
	class CMasternodeEntry {

	private:
        std::string index;
		std::string alias;
		std::string ip;
		std::string privKey;
		std::string txHash;
		std::string outputIndex;
        CTxIn vin;

	public:

        CMasternodeEntry(std::string index, std::string alias, std::string ip, std::string privKey, std::string txHash, std::string outputIndex) {
            this->index = index;
            this->alias = alias;
			this->ip = ip;
			this->privKey = privKey;
			this->txHash = txHash;
			this->outputIndex = outputIndex;
		}

        const std::string& getIndex() const {
            return index;
        }

        void setIndex(const std::string& index) {
            this->index = index;
        }

		const std::string& getAlias() const {
			return alias;
		}

		void setAlias(const std::string& alias) {
			this->alias = alias;
		}

		const std::string& getOutputIndex() const {
			return outputIndex;
		}

        bool castOutputIndex(int& n);
		void setOutputIndex(const std::string& outputIndex) {
			this->outputIndex = outputIndex;
		}

		const std::string& getPrivKey() const {
			return privKey;
		}

		void setPrivKey(const std::string& privKey) {
			this->privKey = privKey;
		}

		const std::string& getTxHash() const {
			return txHash;
		}

		void setTxHash(const std::string& txHash) {
			this->txHash = txHash;
        }

        const std::string& getIp() const {
            return ip;
        }

        void setIp(const std::string& ip) {
            this->ip = ip;
        }

        const CTxIn& getVin() const {
            return vin;
        }

        void setVin(const CTxIn& vin) {
            this->vin = vin;
        }
	};

	CMasternodeConfig() {
		entries = std::vector<CMasternodeEntry>();
	}

	void clear();
    bool read(std::string& strErr);
    bool write();
    void add(string index, std::string alias, std::string ip, std::string privKey, std::string txHash, std::string outputIndex);
    void remove(std::string index);

    CMasternodeEntry findEntryByIp(std::string sAddress) {
        BOOST_FOREACH(CMasternodeEntry mne, entries) {
            if(mne.getIp() == sAddress) {
                return mne;
            }
        }
    }

	std::vector<CMasternodeEntry>& getEntries() 
    {
		return entries;
	}

	int getCount()
	{
		int c = -1;
		BOOST_FOREACH (CMasternodeEntry e, entries) {
						if (e.getAlias() != "") c++;
					}
		return c;
	}

private:
	std::vector<CMasternodeEntry> entries;
};


#endif /* SRC_MASTERNODECONFIG_H_ */

