///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
/*#include "../Many.hpp"
#include <zlib.h>


namespace Langulus::Annies
{

   /// Report a zlib error                                                    
   void pcZLibErrorRelay(int ret) {
      switch (ret) {
      case Z_STREAM_ERROR:
         pcLogError << "ZLIB: Invalid compression level.";
         break;
      case Z_DATA_ERROR:
         pcLogError << "ZLIB: Invalid or incomplete deflate data.";
         break;
      case Z_MEM_ERROR:
         pcLogError << "ZLIB: Out of memory.";
         break;
      case Z_VERSION_ERROR:
         pcLogError << "ZLIB: Version mismatch.";
         break;
      }
   }


   /// Compress data                                                          
   /// You can compress sparse or NON-POD memory easily                       
   /// Do not dereference data, however                                       
   /// Containers should always decompress data before deallocating it.       
   /// Can be used to compress RAM with live links, too                       
   /// Make sure you serialize, before compressing prior to writing a file    
   Count Block::Compress(Block& result, Compression compression_ratio) const {
      if (!IsAllocated())
         return 0;

      // Allocate deflate state                                         
      z_stream strm;
      strm.zalloc = nullptr;
      strm.zfree = nullptr;
      strm.opaque = nullptr;

      int ret = deflateInit(&strm, int(compression_ratio));
      if (ret != Z_OK) {
         // Error while initializing deflator                           
         pcZLibErrorRelay(ret);
         return 0;
      }

      // Source and destination buffers                                 
      auto out = new pcbyte[COMPRESSION_CHUNK];
      Count written = 0;

      // Compress the whole source memory, divided into chunks          
      for (Count i = 0; i < mCount; i += COMPRESSION_CHUNK) {
         strm.avail_in = uInt(mCount - i > COMPRESSION_CHUNK ? COMPRESSION_CHUNK : mCount - i);
         strm.next_in = reinterpret_cast<pcu8*>(const_cast<pcbyte*>(At(i)));
         strm.avail_out = COMPRESSION_CHUNK;
         strm.next_out = reinterpret_cast<pcu8*>(out);

         // Flush only the last chunk                                   
         const int flush = (i + COMPRESSION_CHUNK) >= mCount ? Z_FINISH : Z_NO_FLUSH;
         ret = deflate(&strm, flush);
         if (ret != Z_OK) {
            // Error while initializing deflator                        
            pcZLibErrorRelay(ret);
            delete[] out;
            result.Free();
            return 0;
         }

         // Copy deflated chunk to final output                         
         const auto deflated = COMPRESSION_CHUNK - strm.avail_out;
         result.Allocate(written + deflated);
         written += pcCopyMemory(out, result.At(written), deflated);
      }

      // Clean up and return                                            
      deflateEnd(&strm);
      delete[] out;
      result.mCount = written;
      return written;
   }

   /// Decompress data                                                        
   Count Block::Decompress(Block& result) const {
      if (!IsAllocated())
         return 0;

      // Allocate inflate state                                         
      z_stream strm;
      strm.zalloc = nullptr;
      strm.zfree = nullptr;
      strm.opaque = nullptr;
      strm.avail_in = 0;
      strm.next_in = nullptr;
      int ret = inflateInit(&strm);

      if (ret != Z_OK) {
         // Error while initializing inflator                           
         pcZLibErrorRelay(ret);
         return 0;
      }

      // Source and destination buffers                                 
      auto out = new pcbyte[COMPRESSION_CHUNK];
      Count written = 0;

      // Decompress until deflate stream ends or end of file            
      for (Count i = 0; i < mCount; i += COMPRESSION_CHUNK) {
         strm.avail_in = uInt(mCount - i > COMPRESSION_CHUNK ? COMPRESSION_CHUNK : mCount - i);
         strm.next_in = reinterpret_cast<pcu8*>(const_cast<pcbyte*>(At(i)));
         strm.avail_out = COMPRESSION_CHUNK;
         strm.next_out = reinterpret_cast<pcu8*>(out);

         ret = inflate(&strm, Z_NO_FLUSH);

         // State not clobbered                                         
         assert(ret != Z_STREAM_ERROR);
         switch (ret) {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
            [[fallthrough]];
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
            inflateEnd(&strm);
            pcZLibErrorRelay(ret);
            delete[] out;
            result.Free();
            return 0;
         }

         // Copy inflated chunk to final output                         
         const auto inflated = COMPRESSION_CHUNK - strm.avail_out;
         result.Allocate(written + inflated);
         written += pcCopyMemory(out, result.At(written), inflated);
      }

      // Clean up and return                                            
      inflateEnd(&strm);
      delete[] out;
      result.mCount = written;
      return written;
   }

} // namespace Langulus::Annies*/