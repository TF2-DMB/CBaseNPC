#include <studio.h>

bool CStudioHdr::SequencesAvailable() const
{
	if (m_pStudioHdr->numincludemodels == 0)
	{
		return true;
	}

    return m_pVModel != nullptr;
}

int CStudioHdr::GetNumSeq( void ) const
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->numlocalseq;
	}

	return m_pVModel->m_seq.Count();
}