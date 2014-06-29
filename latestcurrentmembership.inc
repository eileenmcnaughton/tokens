<?php


function latestcurrentmembership_civitoken_declare($token){
  return array(
    $token. '.type' => 'Latest Current Membership Type',
    $token. '.end_date' => 'Latest Current Membership End Date',
    $token. '.start_date' => 'Latest Current Membership Start Date',
    $token. '.fee' => 'Latest Current Membership Fee',
    );
}


function latestcurrentmembership_civitoken_get($cid, &$value, $context){
  //Get contacts group

  $membership = civicrm_api('membership', 'getsingle', array(
    'version' => 3,
    'contact_id' => $cid,
    'sequential' => 1,
    'is_active' => 1,
    'api.membership_type.getsingle' => 1,
    'active_only' => 1,
   ));

   if(!CRM_Utils_Array::value('is_error',$membership)){
     $value['latestcurrentmembership.type'] = $membership['membership_name'];
     $value['latestcurrentmembership.end_date']= date('d-m-Y', strtotime($membership['end_date']));
     $value['latestcurrentmembership.start_date']= date('d-m-Y', strtotime($membership['start_date']));
     $value['latestcurrentmembership.fee']= $membership['api.membership_type.getsingle']['minimum_fee'];
   }
    return $value;

}
