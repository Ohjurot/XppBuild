package com.test;

import java.lang.String;
import java.nio.charset.Charset;
import java.nio.ByteBuffer;
import org.lwjgl.util.xxhash.XXHash;

final class Util{
    public static void print(String msg){
        // Make hash
        ByteBuffer bb = ByteBuffer.wrap(msg.getBytes(Charset.defaultCharset()));
        int hash = XXHash.XXH32(bb, 0);
        
        // Print
        System.out.println(msg);
        System.out.println("Hash: " + hash);
    }
}
