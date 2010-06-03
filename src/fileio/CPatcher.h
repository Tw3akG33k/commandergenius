/*
 * CPatcher.h
 *
 *  Created on: 19.07.2009
 *      Author: gerstrong
 */

#ifndef CPATCHER_H_
#define CPATCHER_H_

#include <list>
#include <string>

class CPatcher {
public:
	CPatcher(int episode, int version,unsigned char *data, const std::string& datadir);
	virtual ~CPatcher();
	
	void patchMemory();
	void patchMemfromFile(const std::string& patch_file_name, long offset);
	void patchMemFromText(unsigned long offset, std::string &patchtext);
	void PatchLevelhint(int level);
	
private:
	
	struct patch_item
	{
		std::string keyword;
		std::string value;
	};

	bool readNextPatchItem(patch_item &PatchItem);

	bool loadPatchfile();
	
	int m_episode;
	int m_version;
	unsigned char *m_data;
	std::string m_datadirectory;
	
	std::list<std::string> m_TextList;
	
};

#endif /* CPATCHER_H_ */
