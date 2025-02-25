function(_Fp16Source_import)
  if(NOT ${DOWNLOAD_FP16})
    set(Fp16Source_FOUND FALSE PARENT_SCOPE)
    return()
  endif(NOT ${DOWNLOAD_FP16})

  nnas_include(ExternalSourceTools)
  nnas_include(OptionTools)

  envoption(EXTERNAL_DOWNLOAD_SERVER "https://github.com")
  # fp16 commit in xnnpack 8b283aa30a31
  envoption(FP16_URL ${EXTERNAL_DOWNLOAD_SERVER}/Maratyszcza/FP16/archive/4dfe081cf6bcd15db339cf2680b9281b8451eeb3.tar.gz)
  ExternalSource_Download(FP16
    DIRNAME FP16
    URL ${FP16_URL})

  set(Fp16Source_DIR ${FP16_SOURCE_DIR} PARENT_SCOPE)
  set(Fp16Source_FOUND TRUE PARENT_SCOPE)
endfunction(_Fp16Source_import)

_Fp16Source_import()
