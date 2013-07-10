
include_directories( ${Qt5Core_INCLUDE_DIRS} )

mpb_test_add( data_block test_main.cpp data/TestBlock.cpp )
mpb_test_link_libraries( data_block ${KORE_TARGET} )

mpb_test_add( parallel_tasklet test_main.cpp parallel/TestTasklet.cpp )
mpb_test_link_libraries( parallel_tasklet ${KORE_TARGET} )
