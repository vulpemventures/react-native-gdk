/* ----------------------------------------------------------------------------
 * Keep this file for compatibility with the prebuilt binaries
 * ----------------------------------------------------------------------------- */

package com.blockstream.libgreen_gdk;

public class GDKJNI {
    // JSON conversion
    public interface JSONConverter {
       Object toJSONObject(final String jsonString);
       String toJSONString(final Object jsonObject);
    }

    private static JSONConverter mJSONConverter = null;

    private static Object toJSONObject(final String jsonString) {
        return mJSONConverter.toJSONObject(jsonString);
    }

    private static String toJSONString(final Object jsonObject) {
        return mJSONConverter.toJSONString(jsonObject);
    }

    // public static void init(JSONConverter _JSONConverter, final Object config) {
    //     mJSONConverter = _JSONConverter;
    //     _internal_GA_init(config);
    // }

    // Notifications
    public interface NotificationHandler {
       void onNewNotification(final Object session, final Object jsonObject);
    }

    private static NotificationHandler mNotificationHandler = null;

    public static void setNotificationHandler(final NotificationHandler notificationHandler) {
        mNotificationHandler = notificationHandler;
    }

    private static void callNotificationHandler(final Object session, final Object jsonObject) {
        if (mNotificationHandler != null)
            mNotificationHandler.onNewNotification(session, jsonObject);
    }

    static final class Obj {
        private final transient long ptr;
        private final int id;
        private Obj(final long ptr, final int id) { this.ptr = ptr; this.id = id; }
        private long get() { return ptr; }
        private int get_id() { return id; }
    }

}
