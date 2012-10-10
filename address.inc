<?php

function address_civitoken_declare($token){
  return array(
    $token. '.address_block' => 'Address Block',
   );
}

/*
 * based on Michael McAndrew's code but updated to use v3 & to only query for required fields
*https://github.com/michaelmcandrew/civicrm-addressblocktoken/blob/master/addressblocktoken.module
*/
function address_civitoken_get($cid, &$value){
  $contact = getMissingAddressFields($cid, $value);
  $value['address.address_block']=nl2br(CRM_Utils_Address::format($contact));
  return $value;
}

/*
 * Function will gather any addition address info required for the mailing label. ie. it
 * will find out the fields required & if there are any for which the key is not currently
 * set it will look them up via api
 * if keys are set but empty this is assumed to reflect underlying info & not looked up
 * note assume that setting is cached in underlying function & doesn't require caching here
 * to prevent multipl calls on the DB
 *
 * @param int $contactID  relevant contact id
 * @param array $values prepopulated values array for given contact
 */
function getMissingAddressFields($contactID, $contactDetails){
  $format = CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME,
      'mailing_format'
  );
  $requiredFields = array();
    preg_match_all('/(?<!\{|\\\\)\{contact.(\w+)\}(?!\})/',
    $format,
    $requiredFields,
    PREG_PATTERN_ORDER
  );
  $customMappings = array(
    'individual_prefix' => 'prefix_id',// abnormal handling in api/bao layer
    'address_name' => 'display_name',//address name not returned
  );
  $requiredFields = array_diff_key(array_flip($requiredFields[1]), $contactDetails);
  foreach ($customMappings as $origField => $substituteField){
    if(isset($requiredFields[$origField])){
      //our anomalies :-( - not sure prefix works as a return on api
      $requiredFields[$substituteField] = 1;
      unset($requiredFields[$origField]);
    }
  }

  if(empty($requiredFields) ||
    (count($requiredFields) == 1
      &&  CRM_Utils_Array::value('contact_type', $contactDetails) == 'Individual'
      && !empty($requiredFields['organization']))){
    // there is no more info to be retrieved
    return $contactDetails;
  }

  $contact = civicrm_api('contact','getsingle', array('version' =>3, 'id' => $contactID,'return' => array_keys($requiredFields)));
  foreach ($customMappings as $origField => $substituteField){
    if(!empty($contact[$substituteField])){
      // individual prefix is an anomoly :-(
      $contact[$origField] = $contact[$substituteField];
      unset($contact[$substituteField]);
    }
  }
  return array_merge($contact, $contactDetails);
}