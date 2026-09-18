///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
//#include "../Many.hpp"


namespace Langulus::Annies
{
   
   /// Encrypt data                                                           
   /*size_t Block::Encrypt(Block& result, const ::std::size_t* keys, const size_t& key_count) const {
      constexpr auto HS = sizeof(Hash);

      // First compress the data, to avoid repeating bytes              
      #if LANGULUS_FEATURE(COMPRESSION)
         auto compressed_size = Compress(result, Compression::Fastest);
      #else
         Clone(result);
         auto compressed_size = result.GetByteSize();
      #endif

      if (0 == compressed_size)
         return 0;

      // Hash the compressed data for validation after decryption       
      const Hash hash = result.GetHash();

      // Append the hash to the back of the compressed memory           
      const auto totalSize = compressed_size + (compressed_size % HS) + HS;
      result.SetType<Byte, true>();
      result.AllocateFresh(result.RequestSize(totalSize));
      result.mCount = totalSize;
      CopyMemory(&hash, result.At(compressed_size), HS);

      // XOR the contents                                               
      //TODO mix with a PRNG
      for (size_t i = 0; i < compressed_size / HS; ++i)
         reinterpret_cast<::std::size_t*>(result.mRaw)[i] ^= keys[i % key_count];

      // Done                                                           
      return result.mCount;
   }

   /// Decrypt data                                                           
   size_t Block::Decrypt(Block& result, const ::std::size_t* keys, const size_t& key_count) const {
      constexpr auto HS = sizeof(Hash);

      // Copy this encrypted data                                       
      Block decrypted;
      Clone(decrypted);

      // XOR the contents to decrypt them                               
      //TODO mix with a PRNG
      for (size_t i = 0; i < mCount / HS; ++i)
         reinterpret_cast<::std::size_t*>(decrypted.mRaw)[i] ^= keys[i % key_count];

      // Get the hash part                                              
      const auto real_size = mCount - HS;
      const auto hash = *reinterpret_cast<::std::size_t*>(decrypted.At(real_size));
      decrypted.mCount = real_size;

      // Hash the compressed data for validation after decryption       
      const auto decoded_hash = decrypted.GetHash();
      if (hash != decoded_hash.mHash) {
         //TODO the hash is small and there may be a collision!

         // Hashes don't match. Decryption failed                       
         // Cleanup and abort, returning zero. If we don't do that      
         // there's undefined behavior coming straight at ya            
         decrypted.Free();
         return 0;
      }

      #if LANGULUS_FEATURE(COMPRESSION)
         // Decompress data                                             
         const auto decompressed_size = decrypted.Decompress(result);

         // Cleanup and we're done                                      
         decrypted.Free();
         return decompressed_size;
      #else
         result = decrypted;
         return result.GetByteSize();
      #endif
   }*/

} // namespace Langulus::Annies
