/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.camera.settings;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.support.v4.app.FragmentActivity;
import android.view.MenuItem;
import android.widget.Toolbar;
import android.widget.Toast;
import android.content.res.Configuration;

import com.android.camera.debug.Log;
import com.android.camera.CameraActivity;

import java.util.Map;

import com.android.camera.settings.SettingsUtil.SelectedPictureSizes;
import com.android.camera.settings.SettingsUtil.SelectedVideoQualities;
import com.android.camera.util.CameraSettingsActivityHelper;
import com.android.camera.util.CameraUtil;
import com.android.camera.util.GoogleHelpHelper;
import com.android.camera2.R;
import com.android.ex.camera2.portability.CameraAgentFactory;
import com.android.ex.camera2.portability.CameraDeviceInfo;
import com.android.ex.camera2.portability.Size;
import com.sprd.camera.storagepath.MultiStorage;
import com.sprd.camera.storagepath.StorageUtil;
import com.ucamera.ucam.modules.BasicModule;
import com.android.camera.util.CameraUtil;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;

/**
 * Provides the settings UI for the Camera app.
 */
public class CameraSettingsActivity extends FragmentActivity {
    /**
     * Used to denote a subsection of the preference tree to display in the
     * Fragment. For instance, if 'Advanced' key is provided, the advanced
     * preference section will be treated as the root for display. This is used
     * to enable activity transitions between preference sections, and allows
     * back/up stack to operate correctly.
     */
    public static final String PREF_SCREEN_EXTRA = "pref_screen_extra";
    public static final String CAMERA_SCOPE = "camera_scope";
    public static final String IS_SUPPORT_FOCUS = "isSupportFouce";
    private static final Log.Tag TAG = new Log.Tag("CameraSettingsActivity");
    public static Context mContext;
    public static ArrayList<Context> contexts = new ArrayList<Context>();
    public String mCameraScope;
    private Toolbar ac;
    private static boolean mResetCamera = false;
    public static boolean isResetUcam = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = CameraSettingsActivity.this;
        contexts.add(mContext);

        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setTitle(R.string.mode_settings);
        ac = (Toolbar)findViewById(com.android.internal.R.id.action_bar);//SPRD:Fix bug 395712

        String prefKey = getIntent().getStringExtra(PREF_SCREEN_EXTRA);
        /* SPRD:Fix bug 390499 @{ */
        String  intentCameraScope = getIntent().getStringExtra(CAMERA_SCOPE);
        Log.i(TAG, "CameraSettingsActivity onCreate prefKey:" + prefKey + ",intentCameraScope:" + intentCameraScope);
        if ((prefKey == null && intentCameraScope != null)
                || (prefKey != null && CameraSettingsFragment.PREF_CATEGORY_ADVANCED
                        .equals(prefKey))) {
            this.mCameraScope = intentCameraScope;
        }
        Log.i(TAG, "CameraSettingsActivity mCameraScope:" + mCameraScope);
        /* @} */
        boolean  isSupportFouce = getIntent().getBooleanExtra(IS_SUPPORT_FOCUS, false);//SPRD:Fix bug 401772
        Log.i(TAG, "CameraSettingsActivity isSupportFouce:" + isSupportFouce);
        CameraSettingsFragment dialog = new CameraSettingsFragment(prefKey,mCameraScope,isSupportFouce);
        getFragmentManager().beginTransaction().replace(android.R.id.content, dialog).commit();
    }

    /* SPRD:Fix bug 395712 @{ */
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            ac.setTitleTextSize(14);
        } else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
            ac.setTitleTextSize(20);
        }
    }
    /* @} */

    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        int itemId = item.getItemId();
        if (itemId == android.R.id.home) {
            finish();
            return true;
        }
        return true;
    }

    public static class CameraSettingsFragment extends PreferenceFragment implements
            OnSharedPreferenceChangeListener {

        public static final String PREF_CAMERA_SETTINGS_CATEGORY = "pref_camera_settings_category";
        public static final String PREF_CATEGORY_RESOLUTION = "pref_category_resolution";
        public static final String PREF_CATEGORY_ADVANCED = "pref_category_advanced";
        public static final String PREF_LAUNCH_HELP = "pref_launch_help";
        private static final Log.Tag TAG = new Log.Tag("SettingsFragment");
        private static DecimalFormat sMegaPixelFormat = new DecimalFormat("##0.0");
        private String[] mCamcorderProfileNames;
        public boolean isSupportGPS = false;
//        public boolean isSupportGPS = android.os.SystemProperties.getBoolean(Keys.KEY_PROPERTY_SUPPORT_GPS, true);
        private CameraDeviceInfo mInfos;
        private String mPrefKey;
        private boolean mGetSubPrefAsRoot = true;
        public String mCameraScope;
        public boolean isSupportFouce = false;
        public CameraSettingsActivity mActivity;
        // Selected resolutions for the different cameras and sizes.
        private SelectedPictureSizes mOldPictureSizesBack;
        private SelectedPictureSizes mOldPictureSizesFront;
        private List<Size> mPictureSizesBack;
        private List<Size> mPictureSizesFront;
        private SelectedVideoQualities mVideoQualitiesBack;
        private SelectedVideoQualities mVideoQualitiesFront;

        private CameraSettingsActivity mSettingsActivity;
        private List<String> mSupportedStorage;
        public CameraSettingsFragment(String prefKey,String mCameraScope,boolean isSupportFouce) {
            mPrefKey = prefKey;
            this.mCameraScope = mCameraScope;
            this.isSupportFouce = isSupportFouce;
        }

        //SPRD:Fix bug 377184
        public CameraSettingsFragment() {
            mPrefKey = null;
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            Bundle arguments = getArguments();
            if (arguments != null) {
                mPrefKey = arguments.getString(PREF_SCREEN_EXTRA);
            }
            Context context = this.getActivity().getApplicationContext();
            addPreferencesFromResource(R.xml.camera_preferences);
            // Allow the Helper to edit the full preference hierarchy, not the sub
            // tree we may show as root. See {@link #getPreferenceScreen()}.
            mGetSubPrefAsRoot = false;
            CameraSettingsActivityHelper.addAdditionalPreferences(this, context);
            mGetSubPrefAsRoot = true;

            mCamcorderProfileNames = getResources().getStringArray(R.array.camcorder_profile_names);
            mInfos = CameraAgentFactory
                    .getAndroidCameraAgent(context, CameraAgentFactory.CameraApi.API_1)
                    .getCameraDeviceInfo();
        }

        @Override
        public void onResume() {
            super.onResume();
            final Activity activity = this.getActivity();

            // Load the camera sizes.
            loadSizes();
            loadStoageDirectories();
            // Make sure to hide settings for cameras that don't exist on this
            // device.
            setVisibilities();

            /* SPRD: Put in the summaries for the setting values. */
            final PreferenceScreen cameraSettingScreen =
                    (PreferenceScreen) findPreference(PREF_CAMERA_SETTINGS_CATEGORY);
            fillEntriesAndSummaries(cameraSettingScreen);
            setPreferenceScreenIntent(cameraSettingScreen);

            // Put in the summaries for the currently set values.
            final PreferenceScreen resolutionScreen =
                    (PreferenceScreen) findPreference(PREF_CATEGORY_RESOLUTION);
            //fillEntriesAndSummaries(resolutionScreen);
            setPreferenceScreenIntent(resolutionScreen);

            final PreferenceScreen advancedScreen =
                (PreferenceScreen) findPreference(PREF_CATEGORY_ADVANCED);
            setPreferenceScreenIntent(advancedScreen);

            /* SPRD: FixBug 380343 Remove this feature.
            Preference helpPref = findPreference(PREF_LAUNCH_HELP);
            helpPref.setOnPreferenceClickListener(
                new OnPreferenceClickListener() {
                    @Override
                    public boolean onPreferenceClick(Preference preference) {
                        GoogleHelpHelper.launchGoogleHelp(activity);
                        return true;
                    }
                });
            */
            Preference resetCamera = findPreference(Keys.KEY_CAMER_RESET);
            resetCamera.setOnPreferenceClickListener(new OnPreferenceClickListener() {
                @Override
                public boolean onPreferenceClick(Preference preference) {
                    showAlertDialog(true);
                    return true;
                }
            });
            Preference resetVideo = findPreference(Keys.KEY_VIDEO_RESET);
            resetVideo.setOnPreferenceClickListener(new OnPreferenceClickListener() {
                @Override
                public boolean onPreferenceClick(Preference preference) {
                    showAlertDialog(false);
                    return true;
                }
            });
            if (CameraUtil.isGifEnnable()) {
                Preference resetGif = findPreference(Keys.KEY_GIF_RESET);
                resetGif.setOnPreferenceClickListener(new OnPreferenceClickListener() {
                    @Override
                    public boolean onPreferenceClick(Preference preference) {
                        showGifAlertDialog();
                        return true;
                    }
                });
            }
            getPreferenceScreen().getSharedPreferences()
                    .registerOnSharedPreferenceChangeListener(this);
        }

        /**
         * Configure home-as-up for sub-screens.
         */
        private void setPreferenceScreenIntent(final PreferenceScreen preferenceScreen) {
            Intent intent = new Intent(getActivity(), CameraSettingsActivity.class);
            intent.putExtra(PREF_SCREEN_EXTRA, preferenceScreen.getKey());
            /* SPRD:Fix bug 390499 @{ */
            Log.i(TAG, "setPreferenceScreenIntent mCameraScope:" + mCameraScope);
            if(PREF_CATEGORY_ADVANCED.equals(preferenceScreen.getKey())){
                Log.i(TAG, "PREF_CATEGORY_ADVANCED mCameraScope:" + mCameraScope);
                if(mCameraScope != null){
                    intent.putExtra(CAMERA_SCOPE, mCameraScope);
                }
                intent.putExtra(IS_SUPPORT_FOCUS, isSupportFouce);//SPRD:Fix bug 401772
            }
            /* @} */
            preferenceScreen.setIntent(intent);
        }

        /**
         * This override allows the CameraSettingsFragment to be reused for
         * different nested PreferenceScreens within the single camera
         * preferences XML resource. If the fragment is constructed with a
         * desired preference key (delivered via an extra in the creation
         * intent), it is used to look up the nested PreferenceScreen and
         * returned here.
         */
        @Override
        public PreferenceScreen getPreferenceScreen() {
            PreferenceScreen root = super.getPreferenceScreen();
            if (!mGetSubPrefAsRoot || mPrefKey == null || root == null) {
                return root;
            } else {
                PreferenceScreen match = findByKey(root, mPrefKey);
                if (match != null) {
                    return match;
                } else {
                    throw new RuntimeException("key " + mPrefKey + " not found");
                }
            }
        }

        private PreferenceScreen findByKey(PreferenceScreen parent, String key) {
            if (key.equals(parent.getKey())) {
                return parent;
            } else {
                for (int i = 0; i < parent.getPreferenceCount(); i++) {
                    Preference child = parent.getPreference(i);
                    if (child instanceof PreferenceScreen) {
                        PreferenceScreen match = findByKey((PreferenceScreen) child, key);
                        if (match != null) {
                            return match;
                        }
                    }
                }
                return null;
            }
        }

        /**
         * Depending on camera availability on the device, this removes settings
         * for cameras the device doesn't have.
         */
        private void setVisibilities() {
            PreferenceGroup resolutions =
                    (PreferenceGroup) findPreference(PREF_CATEGORY_RESOLUTION);
            if (mPictureSizesBack == null) {
                recursiveDelete(resolutions,
                        findPreference(Keys.KEY_PICTURE_SIZE_BACK));
                recursiveDelete(resolutions,
                        findPreference(Keys.KEY_VIDEO_QUALITY_BACK));
            }
            if (mPictureSizesFront == null) {
                recursiveDelete(resolutions,
                        findPreference(Keys.KEY_PICTURE_SIZE_FRONT));
                recursiveDelete(resolutions,
                        findPreference(Keys.KEY_VIDEO_QUALITY_FRONT));
            }
            /* SPRD:Fix bug 386821 @{ */
            if(PREF_CATEGORY_ADVANCED.equals(mPrefKey)){
                SettingsManager settingsManager = new SettingsManager(mContext);
                PreferenceGroup resolutions_advanced =(PreferenceGroup) findPreference(PREF_CATEGORY_ADVANCED);
                int mCameraId = settingsManager.getInteger("_preferences_module_CAM_PhotoModule",Keys.KEY_CAMERA_ID);
                Log.i(TAG, "setVisibilities mCameraId:" + mCameraId);
                if(mCameraId == 1){// front
                    recursiveDelete(resolutions_advanced,findPreference(Keys.KEY_CAMERA_ISO));
                }
                if(!isSupportFouce){//SPRD:Fix bug 401772
                    recursiveDelete(resolutions_advanced,findPreference(Keys.KEY_FOCUS_MODE));
                }
            }
            /* @} */
            PreferenceGroup resolutions_gategory =
                    (PreferenceScreen) findPreference(PREF_CAMERA_SETTINGS_CATEGORY);
            if (!isSupportGPS) {
                recursiveDelete(resolutions_gategory,findPreference(Keys.KEY_RECORD_LOCATION));
            }

            if (!CameraUtil.isGifEnnable()) {
                PreferenceGroup resolutions_advanced =(PreferenceGroup) findPreference(PREF_CATEGORY_ADVANCED);
                recursiveDelete(resolutions_advanced, findPreference(Keys.KEY_CAMERA_GIF_MODE));
            }
            if(!CameraUtil.isVgestureEnnable()){
                PreferenceGroup resolutions_advanced =(PreferenceGroup) findPreference(PREF_CATEGORY_ADVANCED);
                recursiveDelete(resolutions_advanced, findPreference(Keys.KEY_CAMERA_GRID_LINES));
            }
            if(!CameraUtil.isTimeStampEnnable()) {
                PreferenceGroup resolutions_advanced =(PreferenceGroup) findPreference(PREF_CATEGORY_ADVANCED);
                recursiveDelete(resolutions_advanced, findPreference(Keys.KEY_CAMERA_TIME_STAMP));
            }
            if (!CameraUtil.isZslEnable()) {
                PreferenceGroup resolutions_advanced =(PreferenceGroup) findPreference(PREF_CATEGORY_ADVANCED);
                recursiveDelete(resolutions_advanced, findPreference(Keys.KEY_CAMERA_ZSL_DISPLAY));
            }
        }

        /**
         * Recursively go through settings and fill entries and summaries of our
         * preferences.
         */
        private void fillEntriesAndSummaries(PreferenceGroup group) {
            for (int i = 0; i < group.getPreferenceCount(); ++i) {
                Preference pref = group.getPreference(i);
                if (pref instanceof PreferenceGroup) {
                    fillEntriesAndSummaries((PreferenceGroup) pref);
                }
                setSummary(pref);
                setEntries(pref);
            }
        }

        /**
         * Recursively traverses the tree from the given group as the route and
         * tries to delete the preference. Traversal stops once the preference
         * was found and removed.
         */
        private boolean recursiveDelete(PreferenceGroup group, Preference preference) {
            if (group == null) {
                Log.d(TAG, "attempting to delete from null preference group");
                return false;
            }
            if (preference == null) {
                Log.d(TAG, "attempting to delete null preference");
                return false;
            }

            /* SPRD:Fix bug 385791 @{ */
            //if (preference == null) {
            //    return false;
            //}
            /* @} */

            if (group.removePreference(preference)) {
                // Removal was successful.
                return true;
            }

            for (int i = 0; i < group.getPreferenceCount(); ++i) {
                Preference pref = group.getPreference(i);
                if (pref instanceof PreferenceGroup) {
                    if (recursiveDelete((PreferenceGroup) pref, preference)) {
                        return true;
                    }
                }
            }
            return false;
        }

        @Override
        public void onPause() {
            super.onPause();
            mResetCamera = false;
            getPreferenceScreen().getSharedPreferences()
                    .unregisterOnSharedPreferenceChangeListener(this);
        }

        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
            setSummary(findPreference(key));
            setMutexPreference(key);
        }

        /**
         * Set the entries for the given preference. The given preference needs
         * to be a {@link ListPreference}
         */
        private void setEntries(Preference preference) {
            if (!(preference instanceof ListPreference)) {
                return;
            }
            ListPreference listPreference = (ListPreference) preference;

            if (listPreference.getKey().equals(Keys.KEY_PICTURE_SIZE_BACK)) {
                setEntriesForSelection(mPictureSizesBack, listPreference,Keys.KEY_PICTURE_SIZE_BACK);
            } else if (listPreference.getKey().equals(Keys.KEY_PICTURE_SIZE_FRONT)) {
                setEntriesForSelection(mPictureSizesFront, listPreference,Keys.KEY_PICTURE_SIZE_FRONT);
            } else if (listPreference.getKey().equals(Keys.KEY_VIDEO_QUALITY_BACK)) {
                setEntriesForSelection(mVideoQualitiesBack, listPreference);
            } else if (listPreference.getKey().equals(Keys.KEY_VIDEO_QUALITY_FRONT)) {
                setEntriesForSelection(mVideoQualitiesFront, listPreference);
            } else if (listPreference.getKey().equals(Keys.KEY_CAMERA_STORAGE_PATH)) {
                setEntriesForSelectionStorage(mSupportedStorage, listPreference);
            }
        }

        /**
         * Set the summary for the given preference. The given preference needs
         * to be a {@link ListPreference}.
         */
        private void setSummary(Preference preference) {
            if (!(preference instanceof ListPreference)) {
                return;
            }

            ListPreference listPreference = (ListPreference) preference;
            if (listPreference.getKey().equals(Keys.KEY_PICTURE_SIZE_BACK)) {
                setSummaryForSelection(mOldPictureSizesBack, mPictureSizesBack, listPreference);
            } else if (listPreference.getKey().equals(Keys.KEY_PICTURE_SIZE_FRONT)) {
                setSummaryForSelection(mOldPictureSizesFront, mPictureSizesFront, listPreference);
            } else if (listPreference.getKey().equals(Keys.KEY_VIDEO_QUALITY_BACK)) {
                setSummaryForSelection(mVideoQualitiesBack, listPreference);
            } else if (listPreference.getKey().equals(Keys.KEY_VIDEO_QUALITY_FRONT)) {
                setSummaryForSelection(mVideoQualitiesFront, listPreference);
            } else {
                listPreference.setSummary(listPreference.getEntry());
            }
        }

        /**
         * Sets the entries for the given list preference.
         *
         * @param selectedSizes The possible S,M,L entries the user can
         *            choose from.
         * @param preference The preference to set the entries for.
         */
        private void setEntriesForSelection(List<Size> selectedSizes,
                ListPreference preference,String key) {
            if (selectedSizes == null) {
                return;
            }

            SettingsManager sm = new SettingsManager(mContext); 
            String mDefault = null;

            String[] entries = new String[selectedSizes.size()];
            String[] entryValues = new String[selectedSizes.size()];
            for (int i = 0; i < selectedSizes.size(); i++) {
                Size size = selectedSizes.get(i);
                entries[i] = getSizeSummaryString(size);
                entryValues[i] = SettingsUtil.sizeToSetting(size);
                if(i == 0){ 
                    mDefault = entryValues[0];  
                }
            }
            preference.setEntries(entries);
            preference.setEntryValues(entryValues);
            sm.setDefaults(key,mDefault,entryValues);
        }

        /**
         * Sets the entries for the given list preference.
         *
         * @param selectedQualities The possible S,M,L entries the user can
         *            choose from.
         * @param preference The preference to set the entries for.
         */
        private void setEntriesForSelection(SelectedVideoQualities selectedQualities,
                ListPreference preference) {
            if (selectedQualities == null) {
                return;
            }

            // Avoid adding double entries at the bottom of the list which
            // indicates that not at least 3 qualities are supported.
            ArrayList<String> entries = new ArrayList<String>();
            entries.add(mCamcorderProfileNames[selectedQualities.large]);
            if (selectedQualities.medium != selectedQualities.large) {
                entries.add(mCamcorderProfileNames[selectedQualities.medium]);
            }
            if (selectedQualities.small != selectedQualities.medium) {
                entries.add(mCamcorderProfileNames[selectedQualities.small]);
            }
            preference.setEntries(entries.toArray(new String[0]));
        }

        /**
         * Sets the summary for the given list preference.
         *
         * @param oldPictureSizes The old selected picture sizes for small medium and large
         * @param displayableSizes The human readable preferred sizes
         * @param preference The preference for which to set the summary.
         */
        private void setSummaryForSelection(SelectedPictureSizes oldPictureSizes,
                List<Size> displayableSizes, ListPreference preference) {
            if (oldPictureSizes == null) {
                return;
            }

            String setting = preference.getValue();
            Size selectedSize = oldPictureSizes.getFromSetting(setting, displayableSizes);

            preference.setSummary(getSizeSummaryString(selectedSize));
        }

        /**
         * Sets the summary for the given list preference.
         *
         * @param selectedQualities The selected video qualities.
         * @param preference The preference for which to set the summary.
         */
        private void setSummaryForSelection(SelectedVideoQualities selectedQualities,
                ListPreference preference) {
            if (selectedQualities == null) {
                return;
            }
            int selectedQuality = selectedQualities.getFromSetting(preference.getValue());
            preference.setSummary(mCamcorderProfileNames[selectedQuality]);
        }

        /**
         * This method gets the selected picture sizes for S,M,L and populates
         * {@link #mPictureSizesBack}, {@link #mPictureSizesFront},
         * {@link #mVideoQualitiesBack} and {@link #mVideoQualitiesFront}
         * accordingly.
         */
        private void loadSizes() {
            if (mInfos == null) {
                Log.w(TAG, "null deviceInfo, cannot display resolution sizes");
                return;
            }
            // Back camera.
            int backCameraId = SettingsUtil.getCameraId(mInfos, SettingsUtil.CAMERA_FACING_BACK);
            if (backCameraId >= 0) {
                List<Size> sizes = CameraPictureSizesCacher.getSizesForCamera(backCameraId,
                        this.getActivity().getApplicationContext());
                if (sizes != null) {
                    mOldPictureSizesBack = SettingsUtil.getSelectedCameraPictureSizes(sizes,
                            backCameraId);
                    mPictureSizesBack = ResolutionUtil
                            .getDisplayableSizesFromSupported(sizes, true);
                }
                mVideoQualitiesBack = SettingsUtil.getSelectedVideoQualities(backCameraId);
            } else {
                mPictureSizesBack = null;
                mVideoQualitiesBack = null;
            }

            // Front camera.
            int frontCameraId = SettingsUtil.getCameraId(mInfos, SettingsUtil.CAMERA_FACING_FRONT);
            if (frontCameraId >= 0) {
                List<Size> sizes = CameraPictureSizesCacher.getSizesForCamera(frontCameraId,
                        this.getActivity().getApplicationContext());
                if (sizes != null) {
                    mOldPictureSizesFront= SettingsUtil.getSelectedCameraPictureSizes(sizes,
                            frontCameraId);
                    mPictureSizesFront =
                            ResolutionUtil.getDisplayableSizesFromSupported(sizes, false);
                }
                mVideoQualitiesFront = SettingsUtil.getSelectedVideoQualities(frontCameraId);
            } else {
                mPictureSizesFront = null;
                mVideoQualitiesFront = null;
            }
        }

        /**
         * @param size The photo resolution.
         * @return A human readable and translated string for labeling the
         *         picture size in megapixels.
         */
        private String getSizeSummaryString(Size size) {
            Size approximateSize = ResolutionUtil.getApproximateSize(size);
            String megaPixels = sMegaPixelFormat.format((size.width() * size.height()) / 1e6);
            int numerator = ResolutionUtil.aspectRatioNumerator(approximateSize);
            int denominator = ResolutionUtil.aspectRatioDenominator(approximateSize);
            String result = getResources().getString(
                    R.string.setting_summary_aspect_ratio_and_megapixels, numerator, denominator,
                    megaPixels);
            //GST_guojiangping modify size name user static string 20160414 
            //android.util.Log.d("guojiangping", "size.toString() = " + size.toString());
//            if (SystemProperties.get("gst.camera.size.mp", "").equals("aspect"))
//            {
                if (size.toString().equals("Size: (3771 x 2121)")){
            		result = "(16:9) 8MP";
            	}else 
            	if (size.toString().equals("Size: (3264 x 2448)")){
            		result = "(4:3) 8MP";
            	} else if (size.toString().equals("Size: (2592 x 1944)")) {
            		result = "(4:3) 5MP";
            	} else if (size.toString().equals("Size: (2592 x 1458)")) {
            		result = "(16:9) 3.8MP";
            	} else if (size.toString().equals("Size: (2048 x 1536)")) {
            		result = "(4:3) 3.2MP";
            	} else if (size.toString().equals("Size: (1920 x 1088)")) {
            		result = "(16:9) 2MP";
            	} else if (size.toString().equals("Size: (1600 x 1200)")) {
            		result = "(4:3) 2MP";
            	} else if (size.toString().equals("Size: (1280 x 960)")) {
            		result = "(4:3) 1.3MP";
            	} else if (size.toString().equals("Size: (1280 x 720)")) {
            		result = "(16:9) 1MP";
            	} else if (size.toString().equals("Size: (640 x 480)")) {
            		result = "(4:3) VGA";
            	} else if (size.toString().equals("Size: (720 x 480)")) {
            		result = "(16:9) 0.3MP";
            	}else if (size.toString().equals("Size: (864 x 480)")) {
            		result = "(16:9) 0.4MP";
            	}
//            } else if (SystemProperties.get("gst.camera.size.mp", "").equals("true")){
//            	if (size.toString().equals("3264 x 2448")){
//            		result = "8MP";
//            	} else if (size.toString().equals("Size: (2592 x 1944)")) {
//            		result = "5MP";
//              	} else if (size.toString().equals("Size: (2592 x 1458)")) {
//            		result = "3.8MP";
//            	} else if (size.toString().equals("Size: (2048 x 1536)")) {
//            		result = "3.2MP";
//            	} else if (size.toString().equals("Size: (1920 x 1088)")) {
//            		result = "2MP";
//            	} else if (size.toString().equals("Size: (1600 x 1200)")) {
//            		result = "2MP";
//            	} else if (size.toString().equals("Size: (1280 x 960)")) {
//            		result = "1.3MP";
//            	} else if (size.toString().equals("Size: (1280 x 720)")) {
//            		result = "1MP";
//            	} else if (size.toString().equals("Size: (640 x 480)")) {
//            		result = "VGA";
//            	} else if (size.toString().equals("Size: (720 x 480)")) {
//            		result = "0.3MP";
//            	}
//            }
            //GST_guojiangping modify size name user static string 20160414 
            return result;
        }

        /*SPRD: Add Storage check API for supportedStorage */
        public void loadStoageDirectories() {
            List<String> supportedStorage = MultiStorage.getSupportedStorage();
            if (supportedStorage != null) {
                mSupportedStorage = supportedStorage;
            }
        }
        /*SPRD: Add Storage Entries&EntrayValues API for storage setting list */
        public void setEntriesForSelectionStorage(List<String> supportedValue,
                ListPreference preference) {
            if (supportedValue == null) {
                return;
            }
            String[] entries = new String[supportedValue.size()];
            String[] entryValues = new String[supportedValue.size()];
            for (int i = 0; i < supportedValue.size(); i++) {
                String value = supportedValue.get(i);
                entries[i] = getStorageSummeryString(value);
                entryValues[i] = value;
            }
            preference.setEntries(entries);
            preference.setEntryValues(entryValues);
        }
        public String getStorageSummeryString(String value) {
           String entry = null;
           if(MultiStorage.KEY_DEFAULT_INTERNAL.equals(value)){
              entry = getResources().getString(R.string.storage_path_internal);
           } else if (MultiStorage.KEY_DEFAULT_EXTERNAL.equals(value)) {
              entry = getResources().getString(R.string.storage_path_external);
           }
           return entry;
        }

        public void showAlertDialog(final boolean isCamera) {
            AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
            final AlertDialog alertDialog = builder.create();
            builder.setTitle(isCamera ? mContext.getString(R.string.pref_restore_detail) : mContext.getString(R.string.pref_video_restore_detail));
            builder.setMessage(isCamera ? mContext.getString(R.string.restore_message) : mContext.getString(R.string.video_restore_message));
            builder.setPositiveButton(mContext.getString(R.string.restore_done),
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // TODO Auto-generated method stub 229
                            mResetCamera = true;
                            SettingsManager sm = new SettingsManager(mContext);
                            if (isCamera) {
                                isResetUcam = true;
                                sm.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMER_RESET, true);
                                resetCameraSettings(sm);//SPRD:Fix bug 390499
                            } else {
                                sm.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_VIDEO_RESET, true);
                                resetVideoSettings(sm);//SPRD:Fix bug 390499
                            }
                            for (int i = 0; i < contexts.size(); i++) {
                                Context context = contexts.get(i);
                                ((CameraSettingsActivity) context).finish();
                            }
                            contexts.clear();
                           mResetCamera = false;
                        }
                    });
            builder.setNegativeButton(mContext.getString(R.string.restore_cancel),
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // TODO Auto-generated method stub 236
                            alertDialog.dismiss();
                        }
                    });
            AlertDialog dialog = builder.create();
            dialog.show();
        }

        /*SPRD: setMutexPreference method is for Setting function Mutex in setting List*/
        public void setMutexPreference(String key) {
            Preference preference = findPreference(key);
            SettingsManager settingsManager = new SettingsManager(mContext);
            if (preference == null || key == null) {
                return;
            }

            if (key.equals(Keys.KEY_FREEZE_FRAME_DISPLAY)
                    && preference instanceof ManagedSwitchPreference) { //Freezecapture Mutex with Burstcapture
                ManagedSwitchPreference switchPreference = (ManagedSwitchPreference) preference;
                if (switchPreference.isChecked()) {
                    ListPreference burstPreference = (ListPreference)findPreference(Keys.KEY_CAMERA_CONTINUE_CAPTURE);
                    burstPreference.setValue(mContext.getString(R.string.pref_camera_burst_entry_defaultvalue));
                }
                return;
            }
            if (key.equals(Keys.KEY_CAMERA_TIME_STAMP)
                    && preference instanceof ManagedSwitchPreference) {
                ManagedSwitchPreference switchPreference = (ManagedSwitchPreference) preference;
                if (switchPreference.isChecked()) {
                    ListPreference burstPreference = (ListPreference)findPreference(Keys.KEY_CAMERA_CONTINUE_CAPTURE);
                    burstPreference.setValue(mContext.getString(R.string.pref_camera_burst_entry_defaultvalue));
                }
                return;
            }
            if (key.equals(Keys.KEY_CAMERA_CONTINUE_CAPTURE)
                    && preference instanceof ListPreference) { // Burstcapture Mutex with FreezeCapture
                ListPreference listPreference = (ListPreference)preference;
                if (!mContext.getString(R.string.pref_camera_burst_entry_defaultvalue)
                        .equals(listPreference.getValue())) {
                    if(BasicModule.isSceneryModule){
                        ListPreference burstPreference = (ListPreference)findPreference(Keys.KEY_CAMERA_CONTINUE_CAPTURE);
                        burstPreference.setValue(mContext.getString(R.string.pref_camera_burst_entry_defaultvalue));
                        Toast.makeText(mContext,R.string.mode_scenery_burst_cannot_open,Toast.LENGTH_SHORT).show();
                        return;
                    }
                    ManagedSwitchPreference freezePreference =
                            (ManagedSwitchPreference) findPreference(Keys.KEY_FREEZE_FRAME_DISPLAY);
                    freezePreference.setChecked(false);
                    ManagedSwitchPreference timeStampPreference =
                            (ManagedSwitchPreference) findPreference(Keys.KEY_CAMERA_TIME_STAMP);
                    timeStampPreference.setChecked(false);
                    // burst - hdr
                    String flash = settingsManager.getString(mCameraScope, Keys.KEY_FLASH_MODE);// SPRD:Fix bug 415356
                    if(Keys.isHdrOn(settingsManager) || !"off".equals(flash)){//SPRD:Fix bug 408178 && 415356
                        settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                        if(!mResetCamera)
                        Toast.makeText(mContext,R.string.burst_on_hdr_and_flash_off,Toast.LENGTH_SHORT).show();// SPRD:Fix bug 415356
                    }
                    settingsManager.set("_preferences_camera_0",Keys.KEY_FLASH_MODE,"off");//SPRD:Fix bug 406333
                }
                return;
            }
            /* SPRD:Fix bug 381377 @{ */
            if (key.equals(Keys.KEY_WHITE_BALANCE)
                    && preference instanceof ListPreference) { // WhiteBalance Mutex with SceneMode
                ListPreference listPreference = (ListPreference)preference;
                if (!mContext.getString(R.string.pref_camera_whitebalance_default)
                        .equals(listPreference.getValue())){
                    ListPreference sceneModePreference =
                            (ListPreference)findPreference(Keys.KEY_SCENE_MODE);
                    sceneModePreference.setValue(mContext.getString(R.string.pref_camera_scenemode_default));
                    settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);//WhiteBalance mutex with hdr
                }
                /* SPRD:Fix bug 390999 @{ */
                if (!mContext.getString(R.string.pref_camera_whitebalance_default)
                        .equals(listPreference.getValue())) {//WhiteBalance Mutex with HDR
                    settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                }
                /* @} */
                return;
            }
            if (key.equals(Keys.KEY_SCENE_MODE)
                    && preference instanceof ListPreference) { // SceneMode Mutex with WhiteBalance
                ListPreference listPreference = (ListPreference)preference;
                if (!mContext.getString(R.string.pref_camera_scenemode_default)
                        .equals(listPreference.getValue())){
                    ListPreference whiteBalancePreference =
                            (ListPreference)findPreference(Keys.KEY_WHITE_BALANCE);
                    ListPreference meteringPreference =
                            (ListPreference)findPreference(Keys.KEY_CAMER_METERING);
                    whiteBalancePreference.setValue(mContext.getString(R.string.pref_camera_whitebalance_default));
                    meteringPreference.setValue(mContext.getString(R.string.pref_camera_metering_entry_value_frame_average));

                    settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                    settingsManager.set("_preferences_camera_0",Keys.KEY_FLASH_MODE,"off");
                }
                return;
            }
            if (key.equals(Keys.KEY_CAMER_METERING)
                    && preference instanceof ListPreference) { // SceneMode Mutex with WhiteBalance
                ListPreference listPreference = (ListPreference)preference;
                if (!mContext.getString(R.string.pref_camera_metering_entry_value_frame_average)
                        .equals(listPreference.getValue())){
                    ListPreference sceneModePreference =
                            (ListPreference)findPreference(Keys.KEY_SCENE_MODE);

                    sceneModePreference.setValue(mContext.getString(R.string.pref_camera_scenemode_default));
                }
                return;
            }
            /* @} */

            if (key.equals(Keys.KEY_CAMERA_COLOR_EFFECT)) { // color effect - hdr
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                return;
            }
            if (key.equals(Keys.KEY_CAMERA_ISO)) { // ISO - hdr
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                return;
            }

            if (key.equals(Keys.KEY_CAMERA_CONTRAST)) { // contrast - hdr
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                return;
            }
            if (key.equals(Keys.KEY_CAMERA_SATURATION)) { // saturation - hdr
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                return;
            }
            if (key.equals(Keys.KEY_CAMERA_BRIGHTNESS)) { // brightness - hdr
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                return;
            }
            /* SPRD: fix bug 471506 hdr - ZSL @{ */
            if (key.equals(Keys.KEY_CAMERA_ZSL_DISPLAY)) {
                settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
                int countDownDuration = settingsManager.getInteger(
                        SettingsManager.SCOPE_GLOBAL, Keys.KEY_COUNTDOWN_DURATION);
                String flash = settingsManager.getString(mCameraScope, Keys.KEY_FLASH_MODE);
                if (countDownDuration > 0 || !"off".equals(flash)) {
                    settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_COUNTDOWN_DURATION);
                    settingsManager.set("_preferences_camera_0",Keys.KEY_FLASH_MODE,"off");
                    Toast.makeText(mContext,R.string.zsl_on_countdown_off,Toast.LENGTH_SHORT).show();
                }
                return;
            }
            /* @} */
        }

        /*SPRD: resetVideoSettings method is for reset video setting.bug 390499*/
        private void resetVideoSettings(SettingsManager settingsManager) {
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_VIDEO_ENCODE_TYPE);
            Log.i(TAG, "resetVideoSettings mCameraScope:" + mCameraScope);
            if(mCameraScope != null){
                settingsManager.setToDefault("_preferences_camera_0",Keys.KEY_VIDEOCAMERA_FLASH_MODE);//SPRD:Fix bug 399186
                settingsManager.setToDefault("_preferences_camera_1",Keys.KEY_VIDEOCAMERA_FLASH_MODE);//SPRD:Fix bug 399186
            }
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_GRID_LINES);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_RECORD_LOCATION);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_VIDEO_QUALITY_BACK);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_VIDEO_QUALITY_FRONT);
            Map<String, String> mStorage= StorageUtil.supportedRootDirectory();
            String external = mStorage.get(StorageUtil.KEY_DEFAULT_EXTERNAL);
            if (null == external) {
                settingsManager.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_STORAGE_PATH, MultiStorage.KEY_DEFAULT_INTERNAL);
            }else{
                settingsManager.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_STORAGE_PATH, MultiStorage.KEY_DEFAULT_EXTERNAL);
            }
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_VIDEO_SLOW_MOTION);
        }

        /*SPRD: resetCameraSettings method is for reset camera setting.bug 390499*/
        private void resetCameraSettings(SettingsManager settingsManager) {
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_PICTURE_SIZE_FRONT);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_PICTURE_SIZE_BACK);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_RECORD_LOCATION);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_COUNTDOWN_DURATION);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMER_ANTIBANDING);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_SCENE_MODE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_HDR);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_FOCUS_MODE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_WHITE_BALANCE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_JPEG_QUALITY);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_GRID_LINES);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_SHOULD_SHOW_REFOCUS_VIEWER_CLING);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_HDR_PLUS_FLASH_MODE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_SHOULD_SHOW_SETTINGS_BUTTON_CLING);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_FREEZE_FRAME_DISPLAY);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_COLOR_EFFECT);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_CONTINUE_CAPTURE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_ISO);
            Keys.setManualExposureCompensation(settingsManager, false);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_JPEG_QUALITY);
            Map<String, String> mStorage= StorageUtil.supportedRootDirectory();
            String external = mStorage.get(StorageUtil.KEY_DEFAULT_EXTERNAL);
            if (null == external) {
                settingsManager.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_STORAGE_PATH, MultiStorage.KEY_DEFAULT_INTERNAL);
            }else{
                settingsManager.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_STORAGE_PATH, MultiStorage.KEY_DEFAULT_EXTERNAL);
            }
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMER_METERING);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_CONTRAST);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_SATURATION);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_BRIGHTNESS);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_AI_DATECT);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_SHUTTER_SOUND);
            Log.i(TAG, "resetCameraSettings mCameraScope:" + mCameraScope);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_TIME_STAMP);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_VGESTURE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_CAMERA_ZSL_DISPLAY);
            if(mCameraScope != null){
                settingsManager.setToDefault("_preferences_camera_0",Keys.KEY_FLASH_MODE);//SPRD:Fix bug 399186
                settingsManager.setToDefault("_preferences_camera_1",Keys.KEY_FLASH_MODE);//SPRD:Fix bug 399186
             }
            settingsManager.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_MAKEUP_MODE_LEVEL, mContext.getResources().getInteger(R.integer.ucam_makup_default_value));//SPRD:Fix bug 513962
        }

        private void resetGifSettings(SettingsManager settingsManager) {
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_GIF_MODE_PIC_SIZE);
            settingsManager.setToDefault(SettingsManager.SCOPE_GLOBAL, Keys.KEY_GIF_MODE_NUM_SIZE);
        }

        public void showGifAlertDialog() {
            AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
            final AlertDialog alertDialog = builder.create();
            builder.setTitle(mContext.getString(R.string.pref_gif_restore_detail));
            builder.setMessage(mContext.getString(R.string.gif_restore_message));
            builder.setPositiveButton(mContext.getString(R.string.restore_done),
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // TODO Auto-generated method stub 229
                            SettingsManager sm = new SettingsManager(mContext);
                            sm.set(SettingsManager.SCOPE_GLOBAL, Keys.KEY_GIF_RESET, true);
                            resetGifSettings(sm);
                            for (int i = 0; i < contexts.size(); i++) {
                                Context context = contexts.get(i);
                                ((CameraSettingsActivity) context).finish();
                            }
                            contexts.clear();
                        }
                    });
            builder.setNegativeButton(mContext.getString(R.string.restore_cancel),
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // TODO Auto-generated method stub 236
                            alertDialog.dismiss();
                        }
                    });
            AlertDialog dialog = builder.create();
            dialog.show();
        }
    }
}
