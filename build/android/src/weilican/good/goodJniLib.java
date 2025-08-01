/*
  goodJniLib.java
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

package weilican.good;

import android.content.res.AssetManager;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;

public class goodJniLib
{
  public static native void create(String file, AssetManager mgr);
  public static native void init(int width, int height);
  public static native boolean step(int keys, float mousex, float mousey);
  public static native byte[] getSound(int idRes);

  public static String getPath(final Context context, final Uri uri) {
    String filePath = null;

    // Check if the URI is a content URI
    if (uri.getScheme().equals("content")) {
      String[] projection = { MediaStore.Images.Media.DATA };
      Cursor cursor = context.getContentResolver().query(uri, projection, null, null, null);
      if (cursor != null) {
        int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        cursor.moveToFirst();
        filePath = cursor.getString(columnIndex);
        cursor.close();
      }
    } 
    // Check if the URI is a file URI
    else if (uri.getScheme().equals("file")) {
      filePath = uri.getPath();
    }

    // If filePath is still null, try to handle other content types
    if (filePath == null) {
      filePath = handleOtherUriTypes(context, uri);
    }

    return filePath;
  }

  private static String handleOtherUriTypes(Context context, Uri uri) {
    String filePath = null;

    // Handle specific cases for different content providers
    if ("com.google.android.apps.docs.storage".equals(uri.getAuthority())) {
      // Handle Google Drive
      filePath = uri.getPath(); // This may not give a valid path
    } else if ("com.android.providers.media.documents".equals(uri.getAuthority())) {
      // Handle document provider
      String docId = uri.getLastPathSegment();
      String[] split = docId.split(":");
      String type = split[0];

      Uri contentUri = null;
      if ("image".equals(type)) {
        contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
      } else if ("video".equals(type)) {
        contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
      } else if ("audio".equals(type)) {
        contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
      }

      if (contentUri != null) {
        String selection = "_id=?";
        String[] selectionArgs = new String[]{split[1]};
        Cursor cursor = context.getContentResolver().query(contentUri, null, selection, selectionArgs, null);
        if (cursor != null) {
          int columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
          cursor.moveToFirst();
          filePath = cursor.getString(columnIndex);
          cursor.close();
        }
      }
    }

    return filePath;
  }
}
