package com.engpie.eptrunkimpl.repository.auth.impl;

import com.engpie.eptrunkdb.jooq.tables.daos.EpAuthUserDao;
import com.engpie.eptrunkdb.jooq.tables.pojos.EpAuthUser;
import com.engpie.eptrunkdb.jooq.Tables;
import com.engpie.eptrunkimpl.repository.jooq.JooqCrudRepository;
import java.time.Clock;
import java.time.Instant;
import java.time.OffsetDateTime;
import java.util.List;
import java.util.Optional;
import lombok.NonNull;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.jooq.DSLContext;
import org.jooq.Record;
import org.jooq.Record1;
import org.springframework.stereotype.Repository;

/**
 * TODO: add description.
 *
 * @author : imba
 **/
// @Transactional(readOnly = true)
@Slf4j
@Repository
@RequiredArgsConstructor
public class UserCrudRepositoryImpl implements JooqCrudRepository<EpAuthUser, Long> {

  private static final com.engpie.eptrunkdb.jooq.tables.EpAuthUser EP_AUTH_USER = Tables.EP_AUTH_USER;
  private static final com.engpie.eptrunkdb.jooq.tables.EpAuthUserAndAuthRoleTypeRelationship
          EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP = Tables.EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP;

  private final DSLContext dslContext;
  private final EpAuthUserDao epAuthUserDao;
  private final Clock clock;

  @Override
  public long count() {
    return epAuthUserDao.count();
  }

  @Override
  public void delete(EpAuthUser entity) {
    dslContext.deleteFrom(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP)
            .where(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP.ID.eq(entity.getId()))
            .execute();

    epAuthUserDao.delete(entity);
  }

  @Override
  public void deleteAll() {
    dslContext.deleteFrom(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP)
            .execute();

    dslContext.deleteFrom(EP_AUTH_USER).execute();
  }

  @Override
  public void deleteAll(List<? extends EpAuthUser> entities) {
    var entityIds = entities.stream()
            .map(EpAuthUser::getId)
            .toList();

    dslContext.deleteFrom(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP)
            .where(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP.USER_ID.in(entityIds))
            .execute();

    dslContext.deleteFrom(EP_AUTH_USER)
            .where(EP_AUTH_USER.ID.in(entityIds))
            .execute();
  }

  @Override
  public void deleteAllById(List<? extends Long> ids) {
    dslContext.deleteFrom(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP)
            .where(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP.USER_ID.in(ids))
            .execute();

    dslContext.deleteFrom(EP_AUTH_USER)
            .where(EP_AUTH_USER.ID.in(ids))
            .execute();
  }

  @Override
  public void deleteById(Long id) {
    dslContext.deleteFrom(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP)
            .where(EP_AUTH_USER_AND_AUTH_ROLE_TYPE_RELATIONSHIP.USER_ID.eq(id))
            .execute();

    dslContext.deleteFrom(EP_AUTH_USER)
            .where(EP_AUTH_USER.ID.eq(id))
            .execute();
  }

  @Override
  public boolean existsById(Long id) {
    return epAuthUserDao.existsById(id);
  }

  @Override
  public List<EpAuthUser> findAll() {
    return epAuthUserDao.findAll();
  }

  @Override
  public List<EpAuthUser> findAllById(List<Long> longs) {
    return dslContext.select(EP_AUTH_USER)
            .from(EP_AUTH_USER)
            .where(EP_AUTH_USER.ID.in(longs))
            .fetchInto(EpAuthUser.class);
  }

  @Override
  public Optional<EpAuthUser> findById(Long id) {
    return Optional.ofNullable(epAuthUserDao.findById(id));
  }

  //TODO: if "id" not null save new User, if "id" exist, update entity
  @Override
  public EpAuthUser save(@NonNull EpAuthUser entity) {
    return entity.getId() == null
            ? saveEntity(entity)
            : updateEntity(entity);
  }

  @Override
  public List<EpAuthUser> saveAll(List<EpAuthUser> entities) {
    //TODO: create role mapping "ep_auth_user_and_auth_role_type_relationship"
    throw new RuntimeException("TODO: implement me!");
  }


  private EpAuthUser saveEntity(EpAuthUser entity) {
    var offsetDateTimeNow = OffsetDateTime.now(clock);

    var entityId = dslContext.insertInto(EP_AUTH_USER)
            .set(EP_AUTH_USER.EMAIL, entity.getEmail())
            .set(EP_AUTH_USER.PASSWORD, entity.getPassword())
            .set(EP_AUTH_USER.CREATE_DATE, offsetDateTimeNow)
            .set(EP_AUTH_USER.UPDATE_DATE,  offsetDateTimeNow)
            .returningResult(EP_AUTH_USER.ID)
            .fetchOne();

    //TODO: looks like work for extractor class
    var id = Optional.ofNullable(entityId)
            .map(value -> value.get(EP_AUTH_USER.ID))
            .orElse(null);

    //TODO: add mapper
    return new EpAuthUser(
            id,
            entity.getEmail(),
            entity.getPassword(),
            offsetDateTimeNow,
            offsetDateTimeNow
    );
  }


  private EpAuthUser updateEntity(EpAuthUser entity) {
    var offsetDateTimeNow = OffsetDateTime.now(clock);

    var updatedEntity = dslContext.update(EP_AUTH_USER)
            .set(EP_AUTH_USER.EMAIL, entity.getEmail())
            .set(EP_AUTH_USER.PASSWORD, entity.getPassword())
            .set(EP_AUTH_USER.UPDATE_DATE,  offsetDateTimeNow)
            .where(EP_AUTH_USER.ID.eq(entity.getId()))
            .returningResult(EP_AUTH_USER.ID, EP_AUTH_USER.EMAIL, EP_AUTH_USER.PASSWORD, EP_AUTH_USER.CREATE_DATE, EP_AUTH_USER.UPDATE_DATE)
            .fetchOne();

    //TODO: refactoring
    if (updatedEntity == null) {
      return null;
    }

    return new EpAuthUser(
            updatedEntity.get(EP_AUTH_USER.ID),
            updatedEntity.get(EP_AUTH_USER.EMAIL),
            updatedEntity.get(EP_AUTH_USER.PASSWORD),
            updatedEntity.get(EP_AUTH_USER.CREATE_DATE),
            updatedEntity.get(EP_AUTH_USER.UPDATE_DATE)
    );

  }



}
